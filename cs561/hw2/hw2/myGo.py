import sys
import random
import math
from copy import deepcopy

n = 5
max_move = n * n - 1  # The max movement of a Go game   24
komi = n / 2  # Komi rule


def readInput(n, path="input.txt"):
    with open(path, 'r') as f:
        lines = f.readlines()
        piece_type = int(lines[0].strip())
        previous_board = [[int(x) for x in line.rstrip('\n')] for line in lines[1:n+1]]
        board = [[int(x) for x in line.rstrip('\n')] for line in lines[n+1: 2*n+1]]
        return piece_type, previous_board, board


def writeOutput(result, path="output.txt"):
    res = ""
    if result is None:
        res = "PASS"
    else:
        res += str(result[0]) + ',' + str(result[1])
    with open(path, 'w') as f:
        f.write(res)


def compare_board(board1, board2):            # return True if two boards are identical
    for i in range(n):
        for j in range(n):
            if board1[i][j] != board2[i][j]:
                return False
    return True


class GO:
    def __init__(self, piece_type, previous_board, board, n_move):
        self.piece_type = piece_type
        self.previous_board = deepcopy(previous_board)
        self.board = deepcopy(board)
        self.n_move = n_move
        self.died_pieces = []
        for i in range(n):
            for j in range(n):
                if previous_board[i][j] == piece_type and board[i][j] != piece_type:
                    self.died_pieces.append((i, j))

    def game_result(self):
        """
        this property should return:

         1 if player #1 wins
         2  if player #2 wins
         0 if there is a draw
         None if result is unknown

        Returns
        -------
        int

        """
        cnt_1 = sum([self.board[i][j] == 1 for i in range(n) for j in range(n)])
        cnt_2 = sum([self.board[i][j] == 2 for i in range(n) for j in range(n)])
        if cnt_1 > cnt_2 + komi: return 1
        elif cnt_1 < cnt_2 + komi: return 2
        else: return 0

    def is_game_over(self, action="MOVE"):
        """
        boolean indicating if the game is over,
        simplest implementation may just be
        `return self.game_result() is not None`

        Returns
        -------
        boolean

        """
        # Case 1: max move reached
        if self.n_move >= max_move:
            return True
        # Case 2: two players all pass the move.
        if compare_board(self.previous_board, self.board) and action == "PASS":
            return True
        return False

    def move(self, i, j):
        """
        consumes action and returns resulting TwoPlayersAbstractGameState

        Parameters
        ----------
        action: (piecetype, i, j)

        Returns
        -------
        TwoPlayersAbstractGameState

        """
        new_go = self.copy_board()
        valid_place = new_go.valid_place_check(i, j)
        if not valid_place: return None
        new_go.previous_board = deepcopy(new_go.board)
        new_go.board[i][j] = self.piece_type
        new_go.n_move += 1
        new_go.piece_type = 3 - self.piece_type         # switch piecetype
        # self.board = board                            # update_board
        return new_go

    def get_legal_actions(self):
        """
        returns list of legal action at current game state
        Returns
        -------
        list of AbstractGameAction

        """
        possible_placements = []
        for i in range(n):
            for j in range(n):
                if self.valid_place_check(i, j):
                    possible_placements.append((i, j))
        random.shuffle(possible_placements)
        return possible_placements

    def valid_place_check(self, i, j):
        board = self.board

        # Check if the place is in the board range
        if not (0 <= i < n):
            return False
        if not (0 <= j < n):
            return False

        # Check if the place already has a piece
        if board[i][j] != 0:
            return False

        # Copy the board for testing
        test_go = self.copy_board()
        test_board = test_go.board

        # Check if the place has liberty
        test_board[i][j] = self.piece_type  # place in the piece first ?
        test_go.board = test_board
        if test_go.find_liberty(i, j):
            return True

        # If not, remove the died pieces of opponent and check again
        test_go.remove_died_pieces(3 - self.piece_type)  # remove died opponent first !
        if not test_go.find_liberty(i, j):
            return False  # suicide, return false

        else:
            if self.died_pieces and compare_board(self.previous_board, test_go.board):
                return False
        return True

    def copy_board(self):
        return deepcopy(self)

    def get_neighbors(self, i, j):                    # return all the neighbor points of a given position
        board = self.board
        neighbors = []
        if i > 0: neighbors.append((i-1, j))
        if i < n - 1: neighbors.append((i+1, j))
        if j > 0: neighbors.append((i, j-1))
        if j < n - 1: neighbors.append((i, j+1))
        return neighbors

    def detect_neighbor(self, i, j):
        board = self.board
        neighbors = self.get_neighbors(i, j)            # Detect neighbors
        group_allies = []
        for piece in neighbors:
            if board[piece[0]][piece[1]] == board[i][j]:
                group_allies.append(piece)
        return group_allies

    def ally_dfs(self, i, j):                           # return a clustering of connected same piece
        stack = [(i, j)]
        members = []
        while stack:
            piece = stack.pop()
            members.append(piece)
            neighbor_allies = self.detect_neighbor(piece[0], piece[1])
            for ally in neighbor_allies:
                if ally not in stack and ally not in members:
                    stack.append(ally)
        return members

    def find_liberty(self, i, j):                       # return False if the whole cluster has no liberty
        board = self.board
        ally_members = self.ally_dfs(i, j)
        for member in ally_members:
            neighbors = self.get_neighbors(member[0], member[1])
            for piece in neighbors:
                if board[piece[0]][piece[1]] == 0:
                    return True
        return False

    def find_died_pieces(self, piece_type):             # given a piece_type, return all pieces been captured
        board = self.board
        died_pieces = []
        for i in range(n):
            for j in range(n):
                if board[i][j] == piece_type:
                    if not self.find_liberty(i, j):
                        died_pieces.append((i,j))
        return died_pieces

    def remove_died_pieces(self, piece_type):
        died_pieces = self.find_died_pieces(piece_type)
        if not died_pieces: return []
        self.remove_certain_piece(died_pieces)
        return died_pieces

    def remove_certain_piece(self, positions):
        board = self.board
        for piece in positions:
            board[piece[0]][piece[1]] = 0
        self.board = deepcopy(board)
