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


def readfiles(training_dir):
	texts = []
	dir_list = os.listdir(training_dir)
	for sentiment in dir_list:
		first_dir = training_dir + '/' + sentiment
		if os.path.isdir(first_dir):
			sentiment = "N" if sentiment[0] == 'n' else "P"
			second_list = os.listdir(first_dir)
			for reality in second_list:
				second_dir = first_dir + '/' + reality
				if os.path.isdir(second_dir):
					reality = "D" if reality[0] == 'd' else "T"
					third_list = os.listdir(second_dir)
					for fold in third_list:
						third_dir = second_dir + '/' + fold
						if os.path.isdir(third_dir):
							file_list = os.listdir(third_dir)
							for file in file_list:
								file_dir = third_dir + '/' + file
								with open(file_dir, 'r') as f:
									review = f.readline().strip().lower()
									word_list = process_text(review)
									texts.append((word_list, reality, sentiment))
	return texts


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


def build_vocab(texts, threshold):
	vocab = {}
	vocab['N'] = {}
	vocab['P'] = {}
	vocab['D'] = {}
	vocab['T'] = {}
	for file in texts:
		text = file[0]
		real = file[1]
		sent = file[2]
		for word in text:
			if word in vocab[real]:
				vocab[real][word] += 1
			else:
				vocab[real][word] = 1
			if word in vocab[sent]:
				vocab[sent][word] += 1
			else:
				vocab[sent][word] = 1
	for key in vocab:
		vocab[key] = {k: v for k, v in vocab[key].items()}
	return vocab


def build_X2(texts, vocab):
	word_count = ['Nr+', 'Nr-', 'Nn+', 'Nn-', 'X2']
	N_keys = vocab['N'].keys()
	P_keys = vocab['P'].keys()
	D_keys = vocab['D'].keys()
	T_keys = vocab['T'].keys()
	X2 = {}
	X2['N'] = {key: {k: 0 for k in word_count} for key in N_keys}
	X2['P'] = {key: {k: 0 for k in word_count} for key in P_keys}
	X2['D'] = {key: {k: 0 for k in word_count} for key in D_keys}
	X2['T'] = {key: {k: 0 for k in word_count} for key in T_keys}
	
	for review in texts:
		text = review[0]
		real = review[1]
		sent = review[2]
		propagate_X2(text, X2['N'], 'N', sent)
		propagate_X2(text, X2['P'], 'P', sent)
		propagate_X2(text, X2['D'], 'D', real)
		propagate_X2(text, X2['T'], 'T', real)
	for key in X2:
		for word in X2[key]:
			X2[key][word]['X2'] = compute_X2(X2[key][word])
		X2[key] = {w: x2['X2'] for w, x2 in sorted(X2[key].items(), key=lambda item: item[1]['X2'], reverse=True)}
	
	return X2


def propagate_X2(review, X2, category, label):
	for word in X2.keys():
		if word in review:				# found
			if category == label:		# same class
				X2[word]['Nr+'] += 1
			else:						# different class
				X2[word]['Nn+'] += 1
		else:							# not found
			if category == label:		# same class
				X2[word]['Nr-'] += 1
			else:						# different class
				X2[word]['Nn-'] += 1


def compute_X2(word):
	Nr_p = word['Nr+']
	Nr_n = word['Nr-']
	Nn_p = word['Nn+']
	Nn_n = word['Nn-']
	X2 = (Nr_p * Nn_n - Nr_n * Nn_p) / ((Nr_p + Nr_n) * (Nn_p + Nn_n) * (Nr_p + Nn_p) * (Nr_n + Nn_n) + 1)
	return X2


def build_feature(X2):
	features = {}
	features['NP'] = set(list(X2['N'].keys())[:1000] + list(X2['P'].keys())[:1000])
	features['DT'] = set(list(X2['D'].keys())[:1000] + list(X2['T'].keys())[:1000])
	return features


def text2vec(texts, features):
	vec = []
	for review in texts:
		text = review[0]
		real = 1 if review[1] == 'T' else -1
		sent = 1 if review[2] == 'P' else -1
		NP_vec = dict.fromkeys(features['NP'], 0)
		DT_vec = dict.fromkeys(features['DT'], 0)
		for word in text:
			if word in NP_vec:
				NP_vec[word] += 1 / len(text)
			if word in DT_vec:
				DT_vec[word] += 1 / len(text)
		vec.append((DT_vec, real, NP_vec, sent))
	return vec


def perceplearn(texts, loop=10):
	vocab = build_vocab(texts, 7)
	X2 = build_X2(texts, vocab)
	features = build_feature(X2)
	vec = text2vec(texts, features)
	DT_weight = {k: 0 for k in features['DT']}
	DT_avgw = {k: 0 for k in features['DT']}
	NP_weight = {k: 0 for k in features['NP']}
	NP_avgw = {k: 0 for k in features['NP']}
	DT_bias, DT_avgb, NP_bias, NP_avgb = 0, 0, 0, 0
	count = 1
	for i in range(loop):
		shuffle(vec)
		for textvec in vec:
			(DT_vec, real, NP_vec, sent) = textvec
			DT_bias, DT_avgb = update_perceptron(DT_vec, DT_weight, DT_bias, DT_avgw, DT_avgb, real, count)
			NP_bias, NP_avgb = update_perceptron(NP_vec, NP_weight, NP_bias, NP_avgw, NP_avgb, sent, count)
			count += 1			# increment regardless of update

	DT_weight['BIAS'] = DT_bias
	NP_weight['BIAS'] = NP_bias
	DT_avgw['BIAS'] = DT_avgb
	NP_avgw['BIAS'] = NP_avgb
	with open('vanillamodel.txt', 'w') as f:
		f.write(json.dumps(DT_weight) + '\n')
		f.write(json.dumps(NP_weight))

	for word in DT_weight:
		DT_weight[word] -= DT_avgw[word] / count
	for word in NP_weight:
		NP_weight[word] -= NP_avgw[word] / count
	with open('averagedmodel.txt', 'w') as f:
		f.write(json.dumps(DT_weight) + '\n')
		f.write(json.dumps(NP_weight))


def update_perceptron(vec, weight, bias, avg_weight, avg_bias, label, count):
	sum = 0
	for word in vec:
		sum += vec[word] * weight[word]
	sum += bias
	if sum * label > 0:
		return bias, avg_bias
	for word in weight:
		weight[word] += vec[word] * label
		avg_weight[word] += vec[word] * label * count
	bias += label
	avg_bias += label * count
	return bias, avg_bias


if __name__ == '__main__':
	training_dir = sys.argv[1]
	# split_data(dataset_dir, 3)
	training_texts = readfiles(training_dir)
	perceplearn(training_texts, 5)
	
