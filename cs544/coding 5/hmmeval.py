import json
import sys
import os
import re
import math
from hmmdecode import get_model

# Italians: Baseline: 0.89385 Reference: 0.94239 mine: 0.9409640577762849
# Japanese: Baseline: 0.86268 Reference: 0.91889 mine: 0.9188930467322253

def get_accuracy(dev_path, output_path):
	with open(dev_path, 'r') as ref, open(output_path, 'r') as out:
		model = get_model('./hmmmodel.txt')
		vocab = set()
		for emit in model[1].values():
			vocab.update(emit.keys())
		ref_lines = ref.readlines()
		out_lines = out.readlines()
		sent_pair = zip(ref_lines, out_lines)
		total, correct, unseen = 0, 0, 0
		for pair in sent_pair:
			[ref_line, out_line] = pair
			ref_words = ref_line.strip().split()
			ref_tags = []
			words = []
			for tag_word in ref_words:
				slash_pos = tag_word.rfind('/')
				word = tag_word[:slash_pos]
				words.append(word)
				tag = tag_word[slash_pos + 1:]
				ref_tags.append(tag)
			out_words = out_line.strip().split()
			out_tags = []
			for tag_word in out_words:
				slash_pos = tag_word.rfind('/')
				tag = tag_word[slash_pos + 1:]
				out_tags.append(tag)
			assert(len(out_tags) == len(ref_tags))

			for tag_pair in zip(ref_tags, out_tags, words):
				if tag_pair[0] == tag_pair[1]:
					correct += 1
				else:
					# print(tag_pair)
					if tag_pair[2] not in vocab:
						unseen += 1
			total += len(out_tags)
		print(correct / total, unseen / (total - correct))


if __name__ == '__main__':
	dev_path = sys.argv[1]
	output_path = './hmmoutput.txt'
	get_accuracy(dev_path, output_path)