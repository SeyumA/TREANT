#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <unistd.h>

#include "BaggingClassifier.h"
#include "Dataset.h"
#include "DecisionTree.h"
#include "Node.h"
#include "SplitOptimizer.h"
#include "utils.h"

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
              threads] = [](const int argc, char *const *argv)
      -> std::tuple<std::string, std::string, std::size_t, cost_t, int> {
    std::string attackerFile, datasetFile;
    std::size_t maxDepth = 1; // default maxDepth value is 1
    cost_t budget = 0.0f;     // default value is 0.0
    int threads = 1;          // default value is 1, sequential execution
    // parse the arguments
    {
      double bflag = budget;
      int dflag = maxDepth;
      int jflag = threads;
      int c;
      opterr = 0;

      // -a and -f are mandatory
      while ((c = getopt(argc, argv, "a:b:d:f:j:")) != -1)
        switch (c) {
        case 'a':
          attackerFile = std::string(optarg);
          break;
        case 'b':
          bflag = std::stod(std::string(optarg));
          if (budget < 0.0) {
            throw std::runtime_error(
                "Invalid budget argument: it must be >= 0.0");
          }
          budget = bflag;
          break;
        case 'd':
          dflag = std::stoi(std::string(optarg));
          if (dflag < 0) {
            throw std::runtime_error("Invalid depth argument: it must be >= 0");
          }
          maxDepth = dflag;
          break;
        case 'f':
          datasetFile = std::string(optarg);
          break;
        case 'j':
          jflag = std::stoi(std::string(optarg));
          if (jflag < 1) {
            throw std::runtime_error(
                "Invalid threads argument: it must be > 0");
          }
          threads = jflag;
          break;
        case '?':
          if (isprint(optopt)) {
            fprintf(stderr, "Unknown option '-%c'.\n", optopt);
          }
          throw std::runtime_error(
              "Unknown option character, valids are: -a, -b, -d, -f, -j");
        default:
          abort();
        }
    }
    return {attackerFile, datasetFile, maxDepth, budget, threads};
  }(argc, argv);

  // Allocate dataset matrix X and label vector y
  const auto [rows, columnNames] = Dataset::getDatasetInfoFromFile(datasetFile);
  const unsigned cols = columnNames.size();
  // The principal program must manage the memory of X and Y
  feature_t *X = (feature_t *)malloc(sizeof(feature_t) * rows * cols);
  label_t *y = (label_t *)malloc(sizeof(label_t) * rows);
  const auto [isNumerical, notNumericalEntries] =
      Dataset::fillXandYfromFile(X, rows, cols, y, datasetFile);
  std::cout << "The notNumericalEntries size is:" << notNumericalEntries.size()
            << std::endl;

  Dataset dataset(X, rows, cols, y, utils::join(isNumerical, ','),
                  utils::join(notNumericalEntries, ','),
                  utils::join(columnNames, ','));

  if (budget < 0) {
    throw std::runtime_error(
        "ERROR DecisionTree::fit: Invalid "
        "input data (budget must be positive or equal to zero)");
  }
  Attacker attacker(dataset, attackerFile, budget);

  //  std::cout << dataset << std::endl << std::endl;
  std::cout << "The dataset size is:" << dataset.size() << std::endl;
  std::cout << "threads = " << threads << std::endl;
  //

  const bool useICML2019 = false;
  // minimum instances per node (under this threshold the node became a leaf)
  const unsigned minPerNode = 20;
  const bool isAffine = false;

  BaggingClassifier baggingClassifier;
  std::cout << "Fitting the BaggingClassifier\n";
  baggingClassifier.setMaxFeatures(1.0);
  baggingClassifier.fit(dataset, attacker, useICML2019, maxDepth, minPerNode,
                        isAffine);
  std::cout << "End of fitting the BaggingClassifier\n";

  //  const bool useICML2019 = true;

  //  indexes_t rowsIndexes;
  {
    const auto trainTrees =
        [](std::vector<DecisionTree> &trees, const indexes_t &toBeTrained,
           const Dataset &dataset, const Attacker &attacker,
           const unsigned &threads, const bool &useICML2019,
           const unsigned &maxDepth, const unsigned minPerNode,
           const bool isAffine, const indexes_t &rowsIndexes,
           const Impurity impurityType) {
          for (const auto &i : toBeTrained) {
            trees[i].fit(dataset, attacker, threads, useICML2019, maxDepth,
                         minPerNode, isAffine, rowsIndexes, impurityType);
          }
          return;
        };

    unsigned jobs = 4; // jobs used for bagging
    //    threads = 1;  // threads used to build the single decision tree
    unsigned num_trained_trees = 4;
    assert(threads == 1);
    assert(num_trained_trees % jobs == 0);

    {
      const auto start = std::chrono::steady_clock::now();
      std::vector<DecisionTree> trained_trees(num_trained_trees);

      for (unsigned int i = 0; i < num_trained_trees; i++) {
        trained_trees[i].fit(dataset, attacker, threads, useICML2019, maxDepth,
                             minPerNode, isAffine, indexes_t(), Impurity::SSE);
      }
      const auto end = std::chrono::steady_clock::now();

      std::cout << "The first decision tree is:" << std::endl
                << trained_trees[0] << std::endl;

      std::cout << "Time elapsed to fit the decision tree: "
                << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                         start)
                       .count()
                << " milliseconds." << std::endl;
    }
    {
      const auto start = std::chrono::steady_clock::now();
      assert(num_trained_trees >= jobs);
      std::vector<std::thread> tasks;
      // All the decision trees of the forest
      std::vector<DecisionTree> trees(num_trained_trees);
      // Assign tree indexes to jobs
      std::vector<indexes_t> treesPerJob(jobs);
      for (unsigned int jobId = 0; jobId < jobs; jobId++) {
        for (unsigned int j = 0; j < num_trained_trees; j++) {
          if (j % jobs == jobId) {
            treesPerJob[jobId].push_back(j);
          }
        }
      }

      for (unsigned int j = 0; j < jobs; j++) {
        tasks.push_back(std::thread(trainTrees, std::ref(trees), treesPerJob[j],
                                    dataset, attacker, threads, useICML2019,
                                    maxDepth, minPerNode, isAffine, indexes_t(),
                                    Impurity::SSE));
      }
      std::cout << "Joining tasks\n";
      for (auto &t : tasks) {
        t.join();
      }
      std::cout << "All tasks are joint\n";

      const auto end = std::chrono::steady_clock::now();

      std::cout << "Time elapsed to fit the decision tree: "
                << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                         start)
                       .count()
                << " milliseconds." << std::endl;
    }
  }

  // Free memory
  free((void *)X);
  free((void *)y);

  return 0;
}
