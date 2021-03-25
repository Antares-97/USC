import json
import sys
import os
import re
import math
from perceplearn import stop_words


def process_text(review):
	word_list = []
	review = re.sub('[\-\.\?,"+=@#<>*/!;:\^%&\$\(\)]', ' ', review)
	review = re.sub('\d+', ' ', review)
	words = review.split()
	for word in words:
		word = word.strip("?!,'")
		if word in stop_words or len(word) < 3:
			continue
		word_list.append(word)
	return word_list


def text2vec(wordlist, model):
	NP_vec = dict.fromkeys(model['NP'].keys(), 0)
	DT_vec = dict.fromkeys(model['DT'].keys(), 0)		# BIAS included
	for word in wordlist:
		if word in NP_vec:
			NP_vec[word] += 1 / len(wordlist)
		if word in DT_vec:
			DT_vec[word] += 1 / len(wordlist)
	DT_vec['BIAS'] = 1
	NP_vec['BIAS'] = 1
	return DT_vec, NP_vec


def classify(file, model):
	with open(file, 'r') as f:
		review = f.readline().strip().lower()
		word_list = process_text(review)
		DT_vec, NP_vec = text2vec(word_list, model)
		Real = 0
		Sent = 0
		for word in DT_vec:
			Real += DT_vec[word] * model['DT'][word]
		for word in NP_vec:
			Sent += NP_vec[word] * model['NP'][word]
		Real = 'truthful' if Real > 0 else 'deceptive'
		Sent = 'positive' if Sent > 0 else 'negative'
		return Real, Sent


def traverse_data(dev_dir, model):
	with open('./percepoutput.txt', 'w') as f:
		dir_list = os.listdir(dev_dir)
		for sentiment in dir_list:
			first_dir = dev_dir + '/' + sentiment
			if os.path.isdir(first_dir):
				second_list = os.listdir(first_dir)
				for reality in second_list:
					second_dir = first_dir + '/' + reality
					if os.path.isdir(second_dir):
						third_list = os.listdir(second_dir)
						for fold in third_list:
							third_dir = second_dir + '/' + fold
							if os.path.isdir(third_dir):
								file_list = os.listdir(third_dir)
								for file in file_list:
									file_dir = third_dir + '/' + file
									Real, Sent = classify(file_dir, model)
									f.write(Real + "\t" + Sent + "\t" + file_dir + "\r")


def get_model(file):
	model = {}
	model['NP'] = {}
	model['DT'] = {}
	with open(file, 'r') as f:
		json_line = f.readline().strip('\n')
		model['DT'] = json.loads(json_line)
		json_line = f.readline().strip('\n')
		model['NP'] = json.loads(json_line)
	return model


if __name__ == '__main__':
	model_dir = sys.argv[1]
	dev_dir = sys.argv[2]
	model = get_model(model_dir)
	traverse_data(dev_dir, model)
