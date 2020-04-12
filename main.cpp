#include <chrono>
#include <iostream>
#include <queue>

#include "Dataset.h"
#include "DecisionTree.h"
#include "SplitOptimizer.h"

int main() {

  std::string dataset_file_path = "../data/test_training_set.txt";
  Dataset dataset(dataset_file_path);
  std::cout << "The dataset is:" << std::endl << dataset << std::endl;
  std::size_t maxDepth = 4;
  std::size_t budget = 60;
  DecisionTree dt(maxDepth);

  const auto start = std::chrono::steady_clock::now();
  dt.fit(dataset, budget, Impurity::SSE);
  const auto end = std::chrono::steady_clock::now();

  std::cout << "The decision tree is:" << std::endl << dt << std::endl;

  std::cout << "Time elapsed to fit the decision tree: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     start)
                .count() << " milliseconds." << std::endl;
  return 0;
}
