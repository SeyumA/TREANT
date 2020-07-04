#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <unistd.h>

#include "Dataset.h"
#include "DecisionTree.h"
#include "Node.h"
#include "SplitOptimizer.h"
#include "utils.h"

#include <fstream>
#include <future>
#include <iomanip>

static double calculateSum(const Dataset &dataset,
                           const indexes_t &columnIndexes) {
  double sum = 0.0;
  unsigned times = 100000;
  for (unsigned t = 0; t < times; t++) {
    sum = 0.0;
    for (const auto &j : columnIndexes) {
      for (std::size_t i = 0; i < dataset.rows_; i++) {
        sum += dataset(i, j);
      }
    }
  }
  std::cout << "sum = " << sum << std::endl;
  return sum;
}

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
  std::cout << dataset << std::endl << std::endl;
  std::cout << "The dataset size is:" << dataset.size() << std::endl;

  std::cout << "threads = " << threads << std::endl;

  {
    const auto start = std::chrono::steady_clock::now();
    std::vector<std::future<double>> sums;
    for (int t(0); t < threads; t++) {
      indexes_t ids;
      std::cout << "Thread " << t << std::endl;
      for (int j(0); j < (int)cols; j++) {
        if (j % threads == t) {
          ids.push_back(j);
          std::cout << "\t" << j << std::endl;
        }
      }
      sums.emplace_back(
          std::async(std::launch::async, calculateSum, dataset, ids));
    }
    double mean = sums.begin()->get();
    for (int t(1); t < threads; t++) {
      mean += sums[t].get();
    }
    mean /= (dataset.rows_ * dataset.cols_);
    const auto end = std::chrono::steady_clock::now();

    std::cout << "Time elapsed: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       start)
                     .count()
              << " milliseconds." << std::endl;
    std::cout << "mean = " << mean << std::endl;
  }

  return 0;

  //
  DecisionTree dt;
  const bool useICML2019 = false;
  // minimum instances per node (under this threshold the node became a leaf)
  const unsigned minPerNode = 20;
  const bool isAffine = false;
  //  const bool useICML2019 = true;

  {
    const auto start = std::chrono::steady_clock::now();
    dt.fit(dataset, attackerFile, budget, threads, useICML2019, maxDepth,
           minPerNode, isAffine, Impurity::SSE);
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

  dt.save("example.txt");
  DecisionTree dt_copy;
  dt_copy.load("example.txt");

  // Get X as C-order
  feature_t *X_test = (feature_t *)malloc(sizeof(feature_t) * rows * cols);
  std::size_t index = 0;
  for (std::size_t i = 0; i < rows; i++) {
    for (std::size_t j = 0; j < cols; j++) {
      X_test[index] = dataset(i, j);
      index++;
    }
  }
  double *predictions = (double *)malloc(sizeof(double) * rows);
  dt.predict(X_test, rows, cols, predictions, true, false);
  std::cout << "Predictions (rows-wise X):" << std::endl;
  std::cout << std::setprecision(1) << predictions[0];
  for (index_t i = 1; i < rows; i++) {
    std::cout << "," << std::setprecision(1) << predictions[i];
  }
  std::cout << std::endl;
  // test the columns-wise version
  double *predictions_column_wise = (double *)malloc(sizeof(double) * rows);
  dt.predict(X, rows, cols, predictions_column_wise, false, false);
  std::cout << "Predictions (column-wise X):" << std::endl;
  std::cout << std::setprecision(1) << predictions_column_wise[0];
  for (index_t i = 1; i < rows; i++) {
    std::cout << "," << std::setprecision(1) << predictions_column_wise[i];
  }
  std::cout << std::endl;
  // Predict with the decision tree loaded from file
  double *predictions_copy = (double *)malloc(sizeof(double) * rows);
  dt_copy.predict(X_test, rows, cols, predictions_copy, true, false);
  std::cout << "Predictions of dataset loaded from file:" << std::endl;
  std::cout << std::setprecision(1) << predictions_copy[0];
  for (index_t i = 1; i < rows; i++) {
    std::cout << "," << std::setprecision(1) << predictions_copy[i];
  }
  std::cout << std::endl;

  bool areEqual = true;
  for (index_t i = 0; i < rows && areEqual; i++) {
    if (predictions_copy[i] != predictions[i]) {
      std::cout << "index " << i << " is " << predictions[i] << " and "
                << predictions_copy[i] << " on the copy side" << std::endl;
      areEqual = false;
    }
  }
  std::cout << "Are predictions equal? " << areEqual << std::endl;

  // Free memory
  free((void *)X);
  free((void *)y);
  free((void *)X_test);
  free((void *)predictions);
  free((void *)predictions_column_wise);
  free((void *)predictions_copy);

  return 0;
}
