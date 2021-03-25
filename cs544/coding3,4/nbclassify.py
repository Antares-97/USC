import json
import sys
import os
import re
import math
from nblearn import stop_words


def get_model(file):
	f = open(file, 'r')
	json_line = f.readline().strip('\n')
	model = json.loads(json_line)		# should only contain one line
	f.close()
	return model


def merge_dict(dict1, dict2):
	merged = {}
	for key in dict1:
		merged[key] = dict1[key]
		if key in dict2:
			merged[key] += dict2[key]
	for key in dict2:
		merged[key] = dict2[key]
	return merged


def process_text(review):
	review = re.sub('[\-\.\?,"*/!;:\^%&\$\(\)]', ' ', review)
	review = re.sub('\d+', ' ', review)
	word_list = review.split() 
	return word_list


def probability(word_list, vocab, prior):
	total_occ = sum(vocab.values()) - vocab['LEN'] + len(vocab)
	prop = math.log(prior)
	for word in word_list:
		word = word.strip("?!,'")
		if word in stop_words:
			continue
		occ = 1 if word not in vocab else (vocab[word] + 1)
		prop += math.log(occ / total_occ)
	return prop


def classify_4(file, model_4):
	f = open(file, 'r')
	review = f.readline().strip().lower()		# only the first line
	word_list = process_text(review)
	f.close()

	total_len = model_4['N']['T']['LEN'] + model_4['N']['D']['LEN'] + model_4['P']['T']['LEN'] + model_4['P']['D']['LEN']
	prior_NT = model_4['N']['T']['LEN'] / total_len
	prior_ND = model_4['N']['D']['LEN'] / total_len
	prior_PT = model_4['P']['T']['LEN'] / total_len
	prior_PD = model_4['P']['D']['LEN'] / total_len

	p_NT = probability(word_list, model_4['N']['T'], prior_NT)
	p_ND = probability(word_list, model_4['N']['D'], prior_ND)
	p_PT = probability(word_list, model_4['P']['T'], prior_PT)
	p_PD = probability(word_list, model_4['P']['D'], prior_PD)

	p_max = max(p_NT, p_ND, p_PT, p_PD)
	if p_max == p_NT:
		return 'negative', 'truthful'
	elif p_max == p_ND:
		return 'negative', 'deceptive'
	elif p_max == p_PT:
		return 'positive', 'truthful'
	return 'positive', 'deceptive'


def classify_2(file, model_2):
	f = open(file, 'r')
	review = f.readline().strip().lower()		# only the first line
	word_list = process_text(review)
	f.close()

	p_N = probability(word_list, model_2['N'], model_2['N']['LEN'] / model_2['N']['LEN'] + model_2['P']['LEN'])
	p_P = probability(word_list, model_2['P'], model_2['P']['LEN'] / model_2['N']['LEN'] + model_2['P']['LEN'])

	p_D = probability(word_list, model_2['D'], model_2['D']['LEN'] / model_2['D']['LEN'] + model_2['T']['LEN'])
	p_T = probability(word_list, model_2['T'], model_2['T']['LEN'] / model_2['D']['LEN'] + model_2['T']['LEN'])

	Sent = 'negative' if p_N > p_P else 'positive'
	Real = 'deceptive' if p_D > p_T else 'truthful'
	return Sent, Real


def traverse_data(dev_dir, model):
	with open('./nboutput_4.txt', 'w') as f:
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
									Sent, Real = classify_4(file_dir, model)
									f.write(Real + "\t" + Sent + "\t" + file_dir + "\r")


def main_process(dev_dir):
	model_4 = get_model('./nbmodel.txt')
	model_2 = {}
	model_2['N'] = merge_dict(model_4['N']['T'], model_4['N']['D'])
	model_2['P'] = merge_dict(model_4['P']['T'], model_4['P']['D'])
	model_2['D'] = merge_dict(model_4['N']['D'], model_4['P']['D'])
	model_2['T'] = merge_dict(model_4['N']['T'], model_4['P']['T'])
	print(len(model_2['N']), len(model_2['P']), len(model_2['D']), len(model_2['T']))
	# traverse_data(dev_dir, model_4)
	

if __name__ == '__main__':
	dev_dir = sys.argv[1]
	main_process(dev_dir)

