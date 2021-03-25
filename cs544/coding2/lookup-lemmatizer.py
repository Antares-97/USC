 import sys
import re

### Global variables

# Paths for data are read from command line
train_file = sys.argv[1]
test_file = sys.argv[2]

# Counters for lemmas in the training data: word form -> lemma -> count
lemma_count = {}

# Lookup table learned from the training data: word form -> lemma
lemma_max = {}

# Variables for reporting results
training_stats = ['Wordform types' , 'Wordform tokens' , 
                  'Unambiguous types' , 'Unambiguous tokens' , 
                  'Ambiguous types' , 'Ambiguous tokens' , 
                  'Ambiguous most common tokens' , 'Identity tokens']
training_counts = dict.fromkeys(training_stats , 0)     # Create a dictionary with keys from a list, all with the value 0:

test_outcomes = ['Total test items' , 'Found in lookup table' , 
                 'Lookup match' , 'Lookup mismatch' , 
                 'Not found in lookup table' , 'Identity match' , 'Identity mismatch']
test_counts = dict.fromkeys(test_outcomes , 0)

accuracies = {}

### Training: read training data and populate lemma counters

train_data = open (train_file , 'r')

for line in train_data:
    
    # Tab character identifies lines containing tokens
    if re.search ('\t' , line):

        # Tokens represented as tab-separated fields
        field = line.strip().split('\t')

        # Word form in second field, lemma in third field
        form = field[1]
        lemma = field[2]

        ######################################################
        ### Insert code for populating the lemma counts    ###
        ######################################################

        form = form.strip()
        lemma = lemma.strip()

        if form not in lemma_count:
            lemma_count[form] = {}
        if lemma not in lemma_count[form]:
            lemma_count[form][lemma] = 0

        lemma_count[form][lemma] += 1

        # print(form, lemma)

### Model building and training statistics
        
for form in lemma_count.keys():

        ######################################################
        ### Insert code for building the lookup table      ###
        ######################################################
        most_common_lemma = max(lemma_count[form], key=lemma_count[form].get)
        lemma_max[form] = most_common_lemma
        # print(form, lemma_max[form])

        ######################################################
        ### Insert code for populating the training counts ###
        ######################################################
        training_counts['Wordform types'] += 1
        training_counts['Wordform tokens'] += sum(lemma_count[form].values())

        if len(lemma_count[form]) > 1:
            training_counts['Ambiguous types'] += 1
            training_counts['Ambiguous tokens'] += sum(lemma_count[form].values())
            training_counts['Ambiguous most common tokens'] += lemma_count[form][most_common_lemma]
        else:
            training_counts['Unambiguous types'] += 1
            training_counts['Unambiguous tokens'] += lemma_count[form][most_common_lemma]

        if form in lemma_count[form]:               # Outrageous !
            training_counts['Identity tokens'] += lemma_count[form][form]


accuracies['Expected lookup'] = (training_counts['Unambiguous tokens'] + training_counts['Ambiguous most common tokens']) / training_counts['Wordform tokens'] ### Calculate expected accuracy if we used lookup on all items ###

accuracies['Expected identity'] = training_counts['Identity tokens'] / training_counts['Wordform tokens'] ### Calculate expected accuracy if we used identity mapping on all items ###

### Testing: read test data, and compare lemmatizer output to actual lemma
    
test_data = open (test_file , 'r')

for line in test_data:

    # Tab character identifies lines containing tokens
    if re.search ('\t' , line):

        # Tokens represented as tab-separated fields
        field = line.strip().split('\t')

        # Word form in second field, lemma in third field
        form = field[1]
        lemma = field[2]

        ######################################################
        ### Insert code for populating the test counts     ###
        ######################################################
        test_counts['Total test items'] += 1

        lookup_lemma = form         # identity mapping
        if form in lemma_max:
            lookup_lemma = lemma_max[form]
            test_counts['Found in lookup table'] += 1
            if lookup_lemma == lemma:
                test_counts['Lookup match'] += 1
            else:
                test_counts['Lookup mismatch'] += 1
        else:
            test_counts['Not found in lookup table'] += 1
            if lookup_lemma == lemma:
                test_counts['Identity match'] += 1
            else:
                test_counts['Identity mismatch'] += 1

        

accuracies['Lookup'] = test_counts['Lookup match'] / test_counts['Found in lookup table'] ### Calculate accuracy on the items that used the lookup table ###

accuracies['Identity'] = test_counts['Identity match'] / test_counts['Not found in lookup table'] ### Calculate accuracy on the items that used identity mapping ###

accuracies['Overall'] = (test_counts['Lookup match'] + test_counts['Identity match']) / test_counts['Total test items'] ### Calculate overall accuracy ###

### Report training statistics and test results
                
output = open ('lookup-output.txt' , 'w')

output.write ('Training statistics\n')

for stat in training_stats:
    output.write (stat + ': ' + str(training_counts[stat]) + '\n')

for model in ['Expected lookup' , 'Expected identity']:
    output.write (model + ' accuracy: ' + str(accuracies[model]) + '\n')

output.write ('Test results\n')

for outcome in test_outcomes:
    output.write (outcome + ': ' + str(test_counts[outcome]) + '\n')

for model in ['Lookup' , 'Identity' , 'Overall']:
    output.write (model + ' accuracy: ' + str(accuracies[model]) + '\n')

output.close
