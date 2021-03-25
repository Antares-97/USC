import random
import sys
from copy import deepcopy
from myGo import GO, readInput, writeOutput

from go_search import GoSearch
# from go_node import GoNode
from my_node import GoNode


def estimate_n_move(piece_type, previous_board, board):
    black = sum([i.count(1) for i in board])
    white = sum([i.count(2) for i in board])
    n_move = 2 * max(black, white)
    if piece_type == 2:                         # white' round should always be odd
        n_move += 1
    return n_move


if __name__ == "__main__":
    N = 5
    piece_type, previous_board, board = readInput(N)
    if sum([sum(arr) for arr in previous_board]) == 0 and sum([sum(arr) for arr in board]) == 0:
        n_move = 0                  # we play black
    elif sum([sum(arr) for arr in previous_board]) == 0 and sum([sum(arr) for arr in board]) == 1:
        n_move = 1                  # we play white
    else:
        with open('./n_move.txt', 'r') as f:
            line = f.readline().strip()
            n_move = int(line)
    go = GO(piece_type, previous_board, board, n_move)
    root = GoNode(go=go)
    mcts = GoSearch(root)
    best_node = mcts.best_action(400)
    action = best_node.action
    writeOutput(action)
    with open('./n_move.txt', 'w') as f:
        f.write(str(n_move + 2))