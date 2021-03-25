import csv
import numpy as np
from itertools import tee


def pairwise(iterable):
    a, b = tee(iterable)
    next(b, None)
    return zip(a, b)


def sigmoid(z):
    return 1.0 / (1.0 + np.exp(-z))


def sigmoid_prime(z):
    """derivative of sigmoid function"""
    return sigmoid(z) * (1 - sigmoid(z))


def softmax(x):
    '''

    :param x: a list of float values
    :return: a list of probabilities
    '''
    x = np.exp(x)
    s = x / np.sum(x)
    return s


def cross_entropy(p, q):
    '''

    :param p: labeled one-hot encoding  (len(m): batch size)
    :param q: a probability distribution over 10 possible digits
    :return: loss function
    '''
    loss = np.sum(np.multiply(p, np.log(q)))
    loss = - (1.0 / p.shape[0]) * loss
    return loss


def xavier_init(input_num, output_num):
    W = np.random.randn(output_num, input_num) / np.sqrt((input_num + output_num) / 2)
    return W


def model_init(layer_num, nodes_num):
    '''

    :param layer_num: integer, the number of hidden layers
    :param nodes_list: a list, each number in the list stands for the number of nodes in the hidden layer
    :return: a dict, initialized parameters of the model
    '''
    params = {
        'W1': xavier_init(784, nodes_num),
        'b1': np.zeros([nodes_num, 1]),
        'W2': xavier_init(nodes_num, 10),
        'b2': np.zeros([10, 1]),
    }
    n_node_input = 784
    n_node_output = 10
    # nodes_list.insert(0, n_node_input)
    # nodes_list.append(n_node_output)
    # pairs = pairwise(nodes_list)
    # for i, pair in enumerate(pairs):
    #     W = xavier_init(pair[0], pair[1])
    #     b = np.zeros([pair[1], 1])
    #     W_str = 'W%d' % (i + 1)
    #     b_str = 'b%d' % (i + 1)
    #     params[W_str] = W
    #     params[b_str] = b
    # for key in params: print(key, params[key].shape)
    return params


def forward_pass(x_input, params):
    '''

    :param x_input: an numpy array, shape: (784, 1), a column
    :param params: model parameters, W1: (100, 784), b1: (100, 1), W2: (10, 100), b2: (10, 1)
    :return: an numpy array, len = 10, a probability distribution over 10 possible digits
    '''
    x_input = x_input.reshape(-1, 1)
    # print(x_input.shape, x_input)
    cache = {'X': x_input,
             'Z1': np.dot(params['W1'], x_input) + params['b1']}     # (100, 1)
    cache['A1'] = sigmoid(cache['Z1'])                               # (100, 1)
    cache['Z2'] = np.dot(params['W2'], cache['A1']) + params['b2']   # (10, 1)
    cache['A2'] = softmax(cache['Z2'])      # directly apply softmax here (10, 1)
    return cache


def back_propagate(cache, params, x_label):
    '''

    :param cache: a dict, maintaining the results given by each layer
    :param params: model parameter, W1: (100, 784), b1: (100, 1), W2: (10, 100), b2: (10, 1)
    :param x_label: an one-hot coding vector. shape: (1, 10)
    :return:
    '''
    x_label = x_label.reshape(-1, 1)                # shape: (10, 1), 1 column
    db2 = cache['A2'] - x_label                     # (100, 1)
    db1 = sigmoid_prime(cache['Z1']) * np.dot(params['W2'].transpose(), db2)  # (100, 1)
    dW2 = np.dot(db2, cache['A1'].transpose())      # (10, 100)
    dW1 = np.dot(db1, cache['X'].transpose())       # (100, 784)
    return dW1, db1, dW2, db2


def backpropagate(cache, params, x_label):
    x_label = x_label.reshape(-1, 1)                # shape: (10, 1), 1 column
    dZ2 = cache['A2'] - x_label                     # (10, 1)
    dW2 = np.matmul(dZ2, cache['A1'].transpose())   # (10, 100)
    db2 = np.sum(dZ2)
    dA1 = np.matmul(params["W2"].T, dZ2)            # (100, 1)
    dZ1 = dA1 * sigmoid_prime(cache["Z1"])          # (100, 1)
    dW1 = np.matmul(dZ1, cache['X'].transpose())    # (100, 784)
    db1 = np.sum(dZ1)

    return dW1, db1, dW2, db2


def train(training_data, learning_rate, batch_size, params):
    '''

    :param training_data: all the training_data, (x_train, y_train), x_train: (1, 784), y_train: (1, 10)
    :param learning_rate: rate of learning
    :param batch_size: batch size, how many training instance before one parameters update
    :param params: a dict, model parameter
    :return: error rate on training data
    '''
    np.random.shuffle(training_data)
    train_error = 0
    for k in range(0, len(training_data), batch_size):
        batch = training_data[k: k + batch_size]
        sum_gradient_W1 = 0
        sum_gradient_b1 = 0
        sum_gradient_W2 = 0
        sum_gradient_b2 = 0
        for instance in batch:
            x_input, x_label = instance
            cache = forward_pass(x_input.transpose(), params)
            # dW1, db1, dW2, db2 = back_propagate(cache, params, x_label)
            dW1, db1, dW2, db2 = backpropagate(cache, params, x_label)
            sum_gradient_W1 += dW1
            sum_gradient_b1 += db1
            sum_gradient_W2 += dW2
            sum_gradient_b2 += db2
            train_error += int(np.argmax(cache['A2']) != np.argmax(x_label))
        params['W1'] = params['W1'] - learning_rate * sum_gradient_W1 / batch_size
        params['b1'] = params['b1'] - learning_rate * sum_gradient_b1 / batch_size
        params['W2'] = params['W2'] - learning_rate * sum_gradient_W2 / batch_size
        params['b2'] = params['b2'] - learning_rate * sum_gradient_b2 / batch_size
    return train_error / len(training_data)


def test(test_data, params, test_label=None):
    '''

    :param test_data: all the testing data, shape: (10000, 784), every instance: (1, 784)
    :param params: model parameters, weights, bias
    :return: label prediction, shape: (10000, 1)
    '''
    preds = []
    for x_test in test_data:
        cache = forward_pass(x_test, params)
        preds.append(np.argmax(cache['A2']))
    preds = np.asarray(preds, dtype=np.int)
    if test_label is not None:
        test_error = 0
        for pred, truth in zip(preds, test_label):
            test_error += (pred != truth)
        return test_error / len(test_label)
    else:
        with open('./test_predictions.csv', 'w') as output_file:
            for pred in preds:
                output_file.write(str(pred) + '\n')
        return 0


def main_process(training_data, test_data, learning_rate, batch_size, epochs, eval=False):
    '''

    :param training_data: training data, with labels
    :param test_data: test data, without labels
    :param learning_rate: learning rate
    :param batch_size: batch size
    :param epochs: number of loops over the whole dataset
    :param eval: whether evaluate on test dataset
    :return: None
    '''
    nodes_list = 100                          # 1 hidden layer model, with 100 nodes
    params = model_init(1, nodes_list)      # model parameters init
    test_label = None
    if eval:
        with open('./test_label.csv') as f:
            reader = csv.reader(f)
            test_label = [data for data in reader]
            test_label = np.asarray(test_label, dtype=np.int)

    for i in range(epochs):
        np.random.shuffle(training_data)
        train_error_rate = train(training_data, learning_rate, batch_size, params)
        print('epoch #{0}: training error rate: {1}'.format(i + 1, train_error_rate))
        if eval:
            test_error_tate = test(test_data, params, test_label)
            print('epoch #{0}: test error rate: {1}'.format(i + 1, test_error_tate))

    return test(test_data, params, None)
