import json
import sys
import os
import re
from collections import defaultdict

# the separator is always the last slash in the word/tag sequence

begin_state = '<BOS>'
end_state = '<EOS>'


def create_hmm_model(training_path):
	transition = {}			# state transition probability matrix
	emission = {}			# observation emission probability for every state
	transition[end_state] = defaultdict(int)

	with open(training_path, 'r') as f:
		lines = f.readlines()
		for line in lines:
			line = line.strip()
			tagged_words = line.split()

			prev_state = begin_state
			for tag_word in tagged_words:
				slash_pos = tag_word.rfind('/')
				word = tag_word[:slash_pos]
				tag = tag_word[slash_pos + 1:]
				if tag not in transition:
					transition[tag] = defaultdict(int)
					emission[tag] = defaultdict(int)
				transition[tag][prev_state] += 1
				emission[tag][word] += 1
				prev_state = tag
			transition[end_state][prev_state] += 1

	with open('./hmmmodel.txt', 'w') as f:
		f.write(json.dumps(transition) + '\n')
		f.write(json.dumps(emission))


if __name__ == '__main__':
	training_path = sys.argv[1]
	model = create_hmm_model(training_path)
	
