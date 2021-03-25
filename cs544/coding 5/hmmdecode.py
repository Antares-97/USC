import json
import sys
import os
import re
import math
from hmmlearn import begin_state
from hmmlearn import end_state

# reference solution will use add-one smoothing on the transition probabilities 
# no smoothing on the emission probabilities
# for unknown tokens in the test data it will ignore the emission probabilities
# and use the transition probabilities alone. 

negative_inf = - math.inf

def get_model(model_path):
	with open(model_path, 'r') as f:
		json_line = f.readline().strip('\n')
		transition = json.loads(json_line)
		json_line = f.readline().strip('\n')
		emission = json.loads(json_line)
		return (transition, emission)


def do_eval(dev_path, model):
	(transition, emission) = model
	tags_class = {k: len(v) for k, v in emission.items()}
	total = sum(tags_class.values())
	threshold = 0.02
	tags_class = {k: (v / total) for k, v in sorted(tags_class.items(), reverse=True, key=lambda item: item[1])}
	open_class = {k for k, v in tags_class.items() if v > threshold}
	# open_class = set(list(tags_class.keys())[:7])
	states = emission.keys()				# len = N, N: number of states (excluding <EOS>)
	state2idx = {}
	for index, state in enumerate(states):
		state2idx[state] = index			# max index = N - 1
	vocab = set()
	for emit in emission.values():
		vocab.update(emit.keys())			# vocabulary, for all the words seen in the training dataset

	results = []
	with open(dev_path, 'r') as f:
		lines = f.readlines()
		for line in lines:
			line = line.strip()
			word_seq = line.split()
			tag_seq = hmm_tag(word_seq, transition, emission, states, state2idx, vocab, open_class)
			tagged_words = zip(word_seq, tag_seq)
			tagged_words = [pair[0] + '/' + pair[1] for pair in tagged_words]
			results.append(tagged_words)

	with open('./hmmoutput.txt', 'w') as f:
		for line in results:
			string = ' '.join(line)
			f.write(string + '\n')


def hmm_tag(word_seq, transition, emission, states, state2idx, vocab, open_class):
	row = len(states) + 1					# row = N + 1
	column = len(word_seq)
	viterbi = [[negative_inf] * column for i in range(row)]
	backpointer = [[negative_inf] * column for i in range(row)]

	for state in states:					# initialization
		state_id = state2idx[state]
		trans = trans_prop(transition, state, begin_state)
		if word_seq[0] in vocab:
			emit = emit_prop(emission, state, word_seq[0])
			if negative_inf == emit:
				continue
			else:
				viterbi[state_id][0] = trans + emit
		else: 								# OOV words, all emit prop for this word will be 0
			viterbi[state_id][0] = trans
		backpointer[state_id][0] = None

	for i in range(1, column):
		if word_seq[i] in vocab:			# seen words, run for every tags
			for state in states:
				state_id = state2idx[state]
				viterbi[state_id][i] = negative_inf
				backpointer[state_id][i] = None
				emit = emit_prop(emission, state, word_seq[i])
				if negative_inf == emit:	# this tag doesn't generate this word, pass
					continue
				else:
					for s in states:		# check all possible previous tags
						if negative_inf == viterbi[state2idx[s]][i - 1]:
							continue
						prop = viterbi[state2idx[s]][i - 1] + trans_prop(transition, state, s) + emit
						if prop > viterbi[state_id][i]:
							viterbi[state_id][i] = prop
							backpointer[state_id][i] = s
		else:								# for OOV words, we only guess it's an open class tag
			for state in open_class:
				state_id = state2idx[state]
				viterbi[state_id][i] = negative_inf
				backpointer[state_id][i] = None
				for s in states:
					if negative_inf == viterbi[state2idx[s]][i - 1]:
						continue
					prop = viterbi[state2idx[s]][i - 1] + trans_prop(transition, state, s)		# only use transition prop
					if prop > viterbi[state_id][i]:
						viterbi[state_id][i] = prop
						backpointer[state_id][i] = s

	viterbi[row - 1][column - 1] = negative_inf
	backpointer[row - 1][column - 1] = None
	for s in states:
		if negative_inf == viterbi[state2idx[s]][column - 1]:
			continue
		prop = viterbi[state2idx[s]][column - 1] + trans_prop(transition, end_state, s)
		if prop > viterbi[row - 1][column - 1]:
			viterbi[row - 1][column - 1] = prop
			backpointer[row - 1][column - 1] = s

	i = column - 1
	s = backpointer[row - 1][column - 1]
	tag_seq = [s]
	s = backpointer[state2idx[s]][i]
	while s is not None:			# s == None when i == 0
		tag_seq.append(s)
		i = i - 1
		s = backpointer[state2idx[s]][i]
	assert(len(tag_seq) == len(word_seq))
	tag_seq.reverse()
	return tag_seq


def trans_prop(transition, state, prev_state):		# always greater than 0
	occ = 1 if prev_state not in transition[state] else transition[state][prev_state] + 1
	prop = occ / (sum(transition[state].values()) + len(transition[state]) * 0.7)
	return math.log(prop)


def emit_prop(emission, state, obs):
	if obs not in emission[state]:
		return negative_inf
	prop = emission[state][obs] / sum(emission[state].values())
	return math.log(prop)


if __name__ == '__main__':
	model_path = './hmmmodel.txt'
	dev_path = sys.argv[1]
	model = get_model(model_path)
	do_eval(dev_path, model)

