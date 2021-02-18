#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <queue>

#include "utils.h"
#include "BaggingClassifier.h"
#include "Dataset.h"
#include "DecisionTree.h"
#include "Node.h"
#include "SplitOptimizer.h"

#include <cassert>
#include <fstream>
#include <iomanip>
#include <thread>

int main(int argc, char **argv) {

  if (argc < 2) {
    std::cout << "Usage: possible flags are:\n"
              << "-a <name of the attacker json file>, "
              << "-b <budget>, "
              << "-d <max depth>, "
              << "-f <dataset file path>, "
              << "-j <number of threads>\n"
              << "Example:\n./" << argv[0]
              << " -a ../data/attacks.json -b 60 -d 4 -f "
                 "../data/test_training_set_n-1000.txt";
  }

  const auto [attackerFile, datasetFile, maxDepth, budget,
              threads] = utils::parseArguments(argc, argv);

  // Allocate dataset matrix X and label vector y
  const auto [rows, columnNames] = Dataset::getDatasetInfoFromFile(datasetFile);
  const unsigned cols = columnNames.size();
  // The principal program must manage the memory of X and Y
  feature_t *X = (feature_t *)malloc(sizeof(feature_t) * rows * cols);
  label_t *y = (label_t *)malloc(sizeof(label_t) * rows);
  const auto [isNumerical, notNumericalEntries] =
      Dataset::fillXandYfromFile(X, rows, cols, y, datasetFile);
  std::cout << "The notNumericalEntries size is :" << notNumericalEntries.size()
            << std::endl;

  Dataset dataset(X, rows, cols, y, utils::join(isNumerical, ','),
                  utils::join(notNumericalEntries, ','),
                  utils::join(columnNames, ','));

  assert (budget >= 0);
  Attacker attacker(dataset, attackerFile, budget);
  std::cout << "The dataset size is: " << dataset.size() << std::endl;
  std::cout << "The threads used are: " << threads << std::endl;
  //
  DecisionTree dt;
  const bool useICML2019 = false;
  // minimum instances per node (under this threshold the node became a leaf)
  const unsigned minPerNode = 20;
  const bool isAffine = false;
  //  const bool useICML2019 = true;

  {
    const indexes_t rows;
    const auto start = std::chrono::steady_clock::now();
    dt.fit(dataset, attacker, threads, useICML2019, maxDepth,
           minPerNode, isAffine, rows,  Impurity::SSE);
    const auto end = std::chrono::steady_clock::now();

    std::cout << "The decision tree is:" << std::endl << dt << std::endl;

    std::cout << "Time elapsed to fit the decision tree: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       start)
                  .count()
              << " milliseconds." << std::endl;
  }

  std::cout << "Is the decision tree trained? " << dt.isTrained() << std::endl;
  std::cout << "Decision tree height: " << dt.getHeight() << std::endl;
  std::cout << "Decision tree node count: " << dt.getNumberNodes() << std::endl;

  // Free memory
  free((void *)X);
  free((void *)y);

  return 0;
}
