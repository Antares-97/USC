import json
import sys
import os
import re
from random import shuffle
from shutil import copyfile


stop_words = 	["a", "about", "above", "after", "again", "against", 
				 "ain", "all", "am", "an", "and", "any", "are", "aren", 
				 "aren't", "as", "at", "be", "because", "been", "before", 
				 "being", "below", "between", "both", "but", "by", "can", 
				 "couldn", "couldn't", "d", "did", "didn", "didn't", "do", 
				 "does", "doesn", "doesn't", "doing", "don", "don't", 
				 "down", "during", "each", "few", "for", "from", "further", 
				 "had", "hadn", "hadn't", "has", "hasn", "hasn't", "have", 
				 "haven", "haven't", "having", "he", "her", "here", "hers", 
				 "herself", "him", "himself", "his", "how", "i", "if", "in", 
				 "into", "is", "isn", "isn't", "it", "it's", "its", "itself", 
				 "just", "ll", "m", "ma", "me", "mightn", "mightn't", "more", 
				 "most", "mustn", "mustn't", "my", "myself", "needn", "needn't", 
				 "no", "nor", "not", "now", "o", "of", "off", "on", "once", 
				 "only", "or", "other", "our", "ours", "ourselves", "out", 
				 "over", "own", "re", "s", "same", "shan", "shan't", "she", 
				 "she's", "should", "should've", "shouldn", "shouldn't", 
				 "so", "some", "such", "t", "than", "that", "that'll", 
				 "the", "their", "theirs", "them", "themselves", "then", 
				 "there", "these", "they", "this", "those", "through", 
				 "to", "too", "under", "until", "up", "ve", "very", "was", 
				 "wasn", "wasn't", "we", "were", "weren", "weren't", 
				 "what", "when", "where", "which", "while", "who", 
				 "whom", "why", "will", "with", "won", "won't", "wouldn", 
				 "wouldn't", "y", "you", "you'd", "you'll", "you're", 
				 "you've", "your", "yours", "yourself", "yourselves", 
				 "could", "he'd", "he'll", "he's", "here's", "how's", 
				 "i'd", "i'll", "i'm", "i've", "let's", "ought", "she'd", 
				 "she'll", "that's", "there's", "they'd", "they'll", 
				 "they're", "they've", "we'd", "we'll", "we're", "we've", 
				 "what's", "when's", "where's", "who's", "why's", "would"]
stop_words = set(stop_words)


def create_dir():
	path = os.path.exists('./dev_data')
	if not path:
		os.mkdir('./dev_data')
		os.mkdir('./dev_data/negative')
		os.mkdir('./dev_data/negative/deceptive')
		os.mkdir('./dev_data/negative/deceptive/fold/')
		os.mkdir('./dev_data/negative/truthful')
		os.mkdir('./dev_data/negative/truthful/fold/')
		os.mkdir('./dev_data/positive')
		os.mkdir('./dev_data/positive/deceptive')
		os.mkdir('./dev_data/positive/deceptive/fold/')
		os.mkdir('./dev_data/positive/truthful')
		os.mkdir('./dev_data/positive/truthful/fold/')
	path = os.path.exists('./training_data')
	if not path:
		os.mkdir('./training_data')
		os.mkdir('./training_data/negative')
		os.mkdir('./training_data/negative/deceptive')
		os.mkdir('./training_data/negative/deceptive/fold/')
		os.mkdir('./training_data/negative/truthful')
		os.mkdir('./training_data/negative/truthful/fold/')
		os.mkdir('./training_data/positive')
		os.mkdir('./training_data/positive/deceptive')
		os.mkdir('./training_data/positive/deceptive/fold/')
		os.mkdir('./training_data/positive/truthful')
		os.mkdir('./training_data/positive/truthful/fold/')
	return path


def split_data(root_dir, ratio):
	# given the training dataset，split into training dataset and development dataset
	if create_dir():
		return

	dir_list = os.listdir(root_dir)
	for sentiment in dir_list:
		first_dir = root_dir + '/' + sentiment
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
							# file_list.remove('.DS_Store')
							shuffle(file_list)
							dev_len = int(len(file_list) / (ratio + 1))
							dev_list = file_list[:dev_len]
							training_list = file_list[dev_len:]
							for dev_file in dev_list:
								dev_dir = third_dir + '/' + dev_file
								dst_dir = './dev_data/' + sentiment + '/' + reality + '/fold/' + dev_file
								copyfile(dev_dir, dst_dir)
							for training_file in training_list:
								training_dir = third_dir + '/' + training_file
								dst_dir = './training_data/' + sentiment + '/' + reality + '/fold/' + training_file
								copyfile(training_dir, dst_dir)


def traverse_data(training_dir):
	vocab = {}
	dir_list = os.listdir(training_dir)
	for sentiment in dir_list:
		first_dir = training_dir + '/' + sentiment
		if os.path.isdir(first_dir):
			sentiment = "N" if sentiment[0] == 'n' else "P"
			vocab[sentiment] = {}
			second_list = os.listdir(first_dir)
			for reality in second_list:
				second_dir = first_dir + '/' + reality
				if os.path.isdir(second_dir):
					reality = "D" if reality[0] == 'd' else "T"
					vocab[sentiment][reality] = {}
					third_list = os.listdir(second_dir)
					length = 0
					for fold in third_list:
						third_dir = second_dir + '/' + fold
						if os.path.isdir(third_dir):
							file_list = os.listdir(third_dir)
							length += len(file_list)
							for file in file_list:
								file_dir = third_dir + '/' + file
								propagate_dict(file_dir, vocab[sentiment][reality])
					vocab[sentiment][reality]['LEN'] = length
	
	with open('./nbmodel.txt', 'w') as f:
		json_line = json.dumps(vocab)
		f.write(json_line)


def propagate_dict(file, vocab):
	f = open(file, 'r')
	review = f.readline().strip().lower()		# only the first line
	word_list = process_text(review)
	for word in word_list:
		word = word.strip("?!,'")
		if word in stop_words:
			continue
		if word in vocab:
			vocab[word] += 1
		else:
			vocab[word] = 1
	f.close()


def process_text(review):
	review = re.sub('[\-\.\?,"*/!;:\^%&\$\(\)]', ' ', review)
	review = re.sub('\d+', ' ', review)
	word_list = review.split()
	return word_list


if __name__ == '__main__':
	dataset_dir = sys.argv[1]
	# split_data(dataset_dir, 4)
	traverse_data(dataset_dir)
	

