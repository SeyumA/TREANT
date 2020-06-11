import os
import sys

import py_decision_tree  # isort: skip

#filePath = "/home/dg/source/repos/uni/treeant/data/test_training_set_n-1000.txt"
# .encode('ascii') is necessary in python3
maxDepth = 4;
py_dt = py_decision_tree.new(maxDepth)

assert(py_decision_tree.predict(py_dt) == False)

# Get the location of the current .py file
cwd = os.path.dirname(os.path.abspath(__file__))
datasetFile = (cwd + "/data/test_training_set_n-1000.txt").encode('ascii')
attackerFile = (cwd + "/data/attacks.json").encode('ascii')
budget = 0
threads = 4
useICML2019 = True
py_decision_tree.fit(py_dt, datasetFile, attackerFile, budget, threads, useICML2019)

assert py_decision_tree.is_trained(py_dt) == True
print("The decision tree is trained:")
py_decision_tree.pretty_print(py_dt)

py_decision_tree.free(py_dt)
