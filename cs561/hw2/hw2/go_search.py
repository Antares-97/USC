from tqdm import tqdm


class GoSearch(object):
    def __init__(self, node):
        self.root = node                    # starting point of this search

    def _tree_policy(self):
        """
        selects node to run rollout/playout for

        Returns: next node to move on
        -------

        """
        current_node = self.root
        while not current_node.is_terminal_node():      # while not end of the game
            if not current_node.is_fully_expanded():    # if current node still has untried actions
                return current_node.expand()            # expand this node, and return newly-created child node
            else:                                       # cannot move to any other points
                current_node = current_node.best_child()
        return current_node

    def best_action(self, simulations_number):
        """

        Parameters
        ----------
        simulations_number : int
            number of simulations performed to get the best action

        Returns
        -------

        """
        for _ in range(0, simulations_number):
            v = self._tree_policy()                     # choose a child node to rollout
            reward = v.rollout()                        # result when you get the end of the game
            v.backpropagate(reward)
        # to select best child go for exploitation only
        return self.root.best_child(c_param=0.)         # select the best choice child