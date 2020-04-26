#include <chrono>
#include <iostream>
#include <queue>

#include <cstdio>
#include <unistd.h>
#include <omp.h>

#include "Dataset.h"
#include "DecisionTree.h"
#include "SplitOptimizer.h"

int main(int argc, char **argv) {
  static const std::size_t defaultDepth = 1;
  static const cost_t defaultBudget = 0.0f;
  static const unsigned int defaultThreads = 1;

  if (argc < 2) {
    std::cout << "Usage: possible flags are:\n"
              << "-a <name of the attacker json file>, "
              << "-b <budget (default: " << defaultBudget << ")>, "
              << "-d <max depth (default: " << defaultDepth << ")>, "
              << "-f <dataset file path>, "
              << "-j <number of threads (default: " << defaultThreads << ")>\n"
              << "Example:\n./" << argv[0]
              << " -a ../data/attacks.json -b 60 -d 4 -f "
                 "../data/test_training_set_n-1000.txt";
  }
  std::string attackerFile, datasetFile;
  std::size_t maxDepth = defaultDepth; // default maxDepth value is 1
  cost_t budget = defaultBudget;       // default value is 0.0
  // parse the arguments
  {
    int dflag = 0;
    int c;
    opterr = 0;

    while ((c = getopt(argc, argv, "a:bdf:j")) != -1)
      switch (c) {
      case 'a':
        attackerFile = std::string(optarg);
        break;
      case 'b':
        budget = std::stod(std::string(optarg));
        if (budget < 0.0) {
          throw std::runtime_error(
              "Invalid budget argument: it must be >= 0.0");
        }
        break;
      case 'd':
        dflag = std::stoi(std::string(optarg));
        if (dflag < 0) {
          throw std::runtime_error("Invalid depth argument: it must be >= 0");
        }
        break;
      case 'f':
        datasetFile = std::string(optarg);
        break;
      case '?':
        if (optopt == 'c')
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint(optopt))
          fprintf(stderr, "Unknown option '-%c'.\n", optopt);
        else
          fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
        return 1;
      default:
        abort();
      }
  }

  Dataset dataset(datasetFile);
  std::cout << "The dataset is:" << std::endl << dataset << std::endl;
  DecisionTree dt(maxDepth);

  const auto start = std::chrono::steady_clock::now();
  dt.fit(dataset, attackerFile, budget, Impurity::SSE);
  const auto end = std::chrono::steady_clock::now();

  std::cout << "The decision tree is:" << std::endl << dt << std::endl;

  std::cout << "Time elapsed to fit the decision tree: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     start)
                   .count()
            << " milliseconds." << std::endl;
  return 0;
}
