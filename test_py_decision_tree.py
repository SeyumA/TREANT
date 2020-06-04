import os
import sys
#sys.path.append(os.getenv('ACCOUNT_MODULE_PATH'))

import py_decision_tree  # isort: skip

filePath = "/home/dg/source/repos/uni/treeant/data/test_training_set_n-1000.txt"
# .encode('ascii') is necessary in python3 or use b"..." to transform to binary
account1 = account.new(filePath.encode('ascii'))

# TODO: test also predict function
#assert(account.predict(account1) == False)

account.fit(account1)

account.free(account1)

