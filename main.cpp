#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <unistd.h>

#include "Dataset.h"
#include "DecisionTree.h"
#include "SplitOptimizer.h"

class Foo {
public:
  Foo(const double *X, const std::size_t& rows, const std::size_t& cols,
      const double *y)
      : X(X), rows(rows), cols(cols), y(y) {
    if (!(rows > 0 && cols > 0)) {
      throw std::runtime_error("rows and colums in the dataset must be > 0");
    }
  }

  // Read-only accessors -------------------------------------------------------
  // Accessor to X that is stored column-wise
  double operator()(std::size_t i, std::size_t j) const {
    if (i < rows && j < cols) {
      return X[j * rows + i];
    }
    throw std::runtime_error("Array index out of bound, exiting");
  }
  // Accessor to y
  double operator()(std::size_t i) const {
    if (i < rows) {
      return y[i];
    }
    throw std::runtime_error("Array index out of bound, exiting");
  }

  void print() const {
    for (std::size_t i = 0; i < rows; i++) {
      std::cout << this->operator()(i, 0);
      for (std::size_t j = 1; j < cols; j++) {
        std::cout << ' ' << this->operator()(i, j);
      }
      std::cout << std::endl;
    }
  }

private:
  const double *X;
  const std::size_t rows;
  const std::size_t cols;
  const double *y;
};

int main(int argc, char **argv) {

  double xFoo[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const std::size_t rowsFoo = 2;
  const std::size_t colsFoo = 5;
  double yFoo[] = {0, 1};
  Foo foo(xFoo, rowsFoo, colsFoo, yFoo);
  xFoo[0] = 999;
  foo.print();
  std::cout << "Finish printing\n";

  //  double X[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  //  const unsigned rows = 5;
  //  const unsigned cols = 2;
  //  double y[] = {0, 0, 0, 1, 1};
  //  int isNumerical[] = {0, 1};
  //  const std::string notNumericalEntries = "mamma,mia,che,paura,ho";
  //  const std::string columnNames = "age,workclass";
  //
  //  Dataset ds1(X, rows, cols, y, isNumerical, notNumericalEntries,
  //  columnNames); std::cout << ds1.isFeatureNumerical(0) << std::endl;
  //  std::cout << ds1.isFeatureNumerical(1) << std::endl;

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

//  Dataset dataset(datasetFile);
//  std::cout << dataset << std::endl << std::endl;
//  std::cout << "The dataset size is:" << dataset.size() << std::endl;
//  DecisionTree dt(maxDepth);
//  const bool useICML2019 = false;
//  //  const bool useICML2019 = true;
//
//  {
//    const auto start = std::chrono::steady_clock::now();
//    dt.fit(dataset, attackerFile, budget, threads, useICML2019, Impurity::SSE);
//    const auto end = std::chrono::steady_clock::now();
//
//    std::cout << "The decision tree is:" << std::endl << dt << std::endl;
//
//    std::cout << "Time elapsed to fit the decision tree: "
//              << std::chrono::duration_cast<std::chrono::milliseconds>(end -
//                                                                       start)
//                     .count()
//              << " milliseconds." << std::endl;
//  }

//  // ---------------------------------------------------------------------------
//  // Build another dataset with another constructor
//  std::cout << "\n\nBuilding a copy of the dataset" << std::endl;
//  const auto &dsColumns = dataset.getFeatureColumns();
//  const auto &dsLabels = dataset.getLabels();
//  unsigned cols = dsColumns.size();
//  unsigned rows = dsLabels.size();
//  double *X = (double *)malloc(sizeof(double) * cols * rows);
//  for (unsigned i = 0; i < rows; i++) {
//    for (unsigned j = 0; j < cols; j++) {
//      X[i * cols + j] = (dsColumns[j])[i];
//    }
//  }
//  double *y = (double *)malloc(sizeof(double) * rows);
//  for (unsigned i = 0; i < rows; i++) {
//    y[i] = dsLabels[i];
//  }
//  //
//  std::string isNumerical = dataset.isFeatureNumerical(0) ? "True" : "False";
//  for (unsigned j = 1; j < cols; j++) {
//    isNumerical += ',';
//    isNumerical += dataset.isFeatureNumerical(j) ? "True" : "False";
//  }
//  //
//  double key = 0.0;
//  auto nameOpt = dataset.getCategoricalFeatureName(key);
//  std::string notNumericalEntries;
//  while (nameOpt.has_value()) {
//    if (!notNumericalEntries.empty()) {
//      notNumericalEntries += ',';
//    }
//    notNumericalEntries += nameOpt.value();
//    nameOpt = dataset.getCategoricalFeatureName(++key);
//  }
//  //
//  std::string columnNames = dataset.getFeatureName(0);
//  for (unsigned j = 1; j < cols; j++) {
//    columnNames += ',';
//    columnNames += dataset.getFeatureName(j);
//  }
//  Dataset dataset_copy(X, rows, cols, y, isNumerical, notNumericalEntries,
//                       columnNames);
//  DecisionTree dt_copy(maxDepth);
//
//  std::cout << "Fitting again on the copy" << std::endl;
//  {
//    const auto start = std::chrono::steady_clock::now();
//    dt_copy.fit(dataset_copy, attackerFile, budget, threads, useICML2019,
//                Impurity::SSE);
//    const auto end = std::chrono::steady_clock::now();
//
//    std::cout << "The decision tree is:" << std::endl << dt_copy << std::endl;
//
//    std::cout << "Time elapsed to fit the decision tree: "
//              << std::chrono::duration_cast<std::chrono::milliseconds>(end -
//                                                                       start)
//                     .count()
//              << " milliseconds." << std::endl;
//  }
//
//  std::cout << dt_copy << std::endl;
//
//  double *predictions = (double *)malloc(sizeof(double) * rows);
//  dt_copy.predict(X, rows, cols, predictions);
//  std::cout << "Predictions on the same dataset:" << std::endl;
//  for (unsigned i = 0; i < rows; i++) {
//    std::cout << static_cast<int>(predictions[i]) << std::endl;
//  }
//
//  free((void *)X);
//  free((void *)y);
//  free((void *)predictions);
  return 0;
}
