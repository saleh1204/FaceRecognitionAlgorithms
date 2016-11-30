import sys
import os.path
import random
import math

# This is a tiny script to help you creating a CSV file from a face
# database with a similar hierarchie:
# 
#  philipp@mango:~/facerec/data/at$ tree
#  .
#  |-- README
#  |-- s1
#  |   |-- 1.pgm
#  |   |-- ...
#  |   |-- 10.pgm
#  |-- s2
#  |   |-- 1.pgm
#  |   |-- ...
#  |   |-- 10.pgm
#  ...
#  |-- s40
#  |   |-- 1.pgm
#  |   |-- ...
#  |   |-- 10.pgm
#

if __name__ == "__main__":
    
    if len(sys.argv) != 5:
        print "usage: create_csv <base_path> <trainingFile.csv> <testingFile.csv> <TestingSetPercentage>"
        sys.exit(1)
    
    BASE_PATH=sys.argv[1]
    SEPARATOR=";"
    trainingFile=open(sys.argv[2], 'w')
    testingFile=open(sys.argv[3], 'w')
    TEST_PERCENTAGE=int(sys.argv[4])

    label = 0
    for dirname, dirnames, filenames in os.walk(BASE_PATH):
        for subdirname in dirnames:
            subject_path = os.path.join(dirname, subdirname)
            total = []
            train_set = []
            test_set = []
            for filename in os.listdir(subject_path):
                abs_path = "%s/%s" % (subject_path, filename)
		total.append("%s%s%d" % (abs_path, SEPARATOR, label))
                #print "%s%s%d" % (abs_path, SEPARATOR, label)
            random.shuffle(total)
            totalNumber = len(total)
            testNumber = int(math.ceil(totalNumber * TEST_PERCENTAGE / 100))
            #print testNumber
            test_set  = total[:testNumber]
            train_set = total[testNumber:]
            # Print out the values into two files
            for item in train_set:
#                print item
		trainingFile.write(item+"\n")
            for val in test_set:
                #print val
                testingFile.write(val+"\n")
            label = label + 1

    trainingFile.close()
    testingFile.close()
