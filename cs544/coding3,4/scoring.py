import os


def antonym(word):
	if word == 'positive':
		return 'negative'
	elif word == 'negative':
		return 'positive'
	elif word == 'truthful':
		return 'deceptive'
	return 'truthful'


def statistics(file):
	keys = ['TP', 'FP', 'FN', 'TN']
	stat = {}
	stat['positive'] = dict.fromkeys(keys , 0)
	stat['negative'] = dict.fromkeys(keys , 0)
	stat['truthful'] = dict.fromkeys(keys , 0)
	stat['deceptive'] = dict.fromkeys(keys , 0)
	
	with open(file, 'r') as f:
		lines = f.read().strip('\n').split('\n')
		for line in lines:
			[reality, sentiment, path] = line.split('\t')
			path = path.split('/')
			Sent = path[-4]
			Real = path[-3]
			if sentiment == Sent:
				stat[Sent]['TP'] += 1
				stat[antonym(Sent)]['TN'] += 1
			else:
				stat[Sent]['FN'] += 1
				stat[antonym(Sent)]['FP'] += 1
			if reality == Real:
				stat[Real]['TP'] += 1
				stat[antonym(Real)]['TN'] += 1
			else:
				stat[Real]['FN'] += 1
				stat[antonym(Real)]['FP'] += 1

	F1_p = F1_score(stat, 'positive')
	F1_n = F1_score(stat, 'negative')
	F1_t = F1_score(stat, 'truthful')
	F1_d = F1_score(stat, 'deceptive')
	print('positive: ', F1_p)
	print('negative: ', F1_n)
	print('truthful: ', F1_t)
	print('deceptive: ', F1_d)
	print('average: ', (F1_p + F1_n + F1_t + F1_d) / 4)


def precision(stat, category):
	if category not in stat:
		return 0
	return stat[category]['TP'] / (stat[category]['TP'] + stat[category]['FP'])


def recall(stat, category):
	if category not in stat:
		return 0
	return stat[category]['TP'] / (stat[category]['TP'] + stat[category]['FN'])


def F1_score(stat, category):
	prec = precision(stat, category)
	recl = recall(stat, category)
	return 2*prec*recl / (prec + recl)


if __name__ == '__main__':
	statistics('percepoutput.txt')
