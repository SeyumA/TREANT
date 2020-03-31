#include <iostream>
#include <queue>

#include "Dataset.h"
#include "DecisionTree.h"
#include "SplitOptimizer.h"

int main() {

  std::string dataset_file_path =
      "/home/dg/source/repos/uni/treeant/data/test_training_set.txt";
  Dataset dataset(dataset_file_path);
  std::cout << "The dataset is:" << std::endl << dataset << std::endl;
  std::size_t maxDepth = 4;
  std::size_t budget = 60;
  DecisionTree dt(maxDepth);
  dt.fit(dataset, budget, Impurity::SSE);
  std::cout << "The decision tree is:" << std::endl << dt << std::endl;

  std::cout << "Is the decision tree trained: " << dt.isTrained() << std::endl;
  return 0;
}
