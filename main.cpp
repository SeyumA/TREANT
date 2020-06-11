#include <algorithm>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <queue>
#include <unistd.h>

#include "Dataset.h"
#include "DecisionTree.h"
#include "SplitOptimizer.h"


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

  Dataset dataset(datasetFile);
  std::cout << "The dataset size is:" << dataset.size() << std::endl;
  DecisionTree dt(maxDepth);
  const bool useICML2019 = true;

  const auto start = std::chrono::steady_clock::now();
  dt.fit(dataset, attackerFile, budget, threads, useICML2019, Impurity::SSE);
  const auto end = std::chrono::steady_clock::now();

  std::cout << "The decision tree is:" << std::endl << dt << std::endl;

  std::cout << "Time elapsed to fit the decision tree: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     start)
                   .count()
            << " milliseconds." << std::endl;
  return 0;
}
