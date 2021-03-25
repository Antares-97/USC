import random
import sys
from collections import defaultdict
import math
from copy import deepcopy
from myGo import GO


class GoNode:
    def __init__(self, go, parent=None, action=None):
        assert go
        self.go = go                                # current GO instance
        self.parent = parent                        # parent node of this node
        self.action = action                        # the action taken by its parent node to get to this node
        self.children = []                          # children nodes
        self._number_of_visits = 0.                 # n
        self._results = defaultdict(int)            # index 1 for winning times of black, 2 for white, 0 for draw
        self._untried_actions = None                # valid moves for current GO instance

    def untried_actions(self):                      # return all valid position for next move
        if self._untried_actions is None:
            self._untried_actions = self.go.get_legal_actions()
        return self._untried_actions                # init once, every next call it returns the list

    def q(self):
        wins = self._results[self.go.piece_type]
        loses = self._results[3 - self.go.piece_type]
        return wins - loses

    def n(self):
        return self._number_of_visits + 1

    def expand(self):
        '''
        create a child node for current node
        :return: a newly-created child node
        '''
        (i, j) = self.untried_actions().pop()                 # pop the first valid position from the list
        next_go = self.go.move(i, j)
        assert next_go is not None
        child_node = GoNode(
            next_go, parent=self, action=(i, j)
        )
        self.children.append(child_node)
        return child_node                                    # return this expanded child node

    def is_fully_expanded(self):
        return len(self.untried_actions()) == 0

    def is_terminal_node(self):              # check if this is a leaf node (end of the game)
        '''

        :param action: the action taken from the parent node to get to this node
        :return: True if this is the end of game (current node is a leaf node)
        '''
        return self.go.is_game_over()

    def best_child(self, c_param=0.3):
        choices_weights = [
            (c.q() / c.n()) + c_param * math.sqrt((math.log(self.n() + 1) / c.n()))
            for c in self.children
        ]
        return self.children[choices_weights.index(max(choices_weights))]

    def rollout(self):
        '''
        go down through this node, simulate opponent's move by randomly choosing
        :return: result of simulation. 1 if black wins, 2 if white wins, 0 for draw
        '''
        current_go = self.go
        while not current_go.is_game_over():
            possible_moves = current_go.get_legal_actions()         # could be an empty set
            if len(possible_moves) == 0:                            # leaf node, break the loop
                break                                               # the board does not change if we pass
            (i, j) = possible_moves[random.randint(0, len(possible_moves) - 1)]
            current_go = current_go.move(i, j)                      # n_move + 1, switch piecetype
        return current_go.game_result()                    # return 1 if black wins, 2 if white wins, 0 for draw

    def backpropagate(self, result):
        '''

        :param result: 1 if black wins, 2 if white wins, 0 for draw
        :return: None
        '''
        self._number_of_visits += 1
        reward = 3 if result == 1 else 1
        self._results[result] += reward
        if self.parent:
            self.parent.backpropagate(result)