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
#include <iomanip>

class Foo {
public:
  Foo(double data) : data(data), left(0), right(0) {}
  Foo(double data, Foo *left, Foo *right)
      : data(data), left(left), right(right) {}

  ~Foo() {
    std::cout << "Deleting node with " << data << std::endl;
    delete left;
    delete right;
  }

  void print() const {
    std::cout << "data is: "
              << std::setprecision(std::numeric_limits<double>::max_digits10)
              << data << std::endl;
  }

private:
  double data;

public:
  Foo *left;
  Foo *right;
};

void printFoo(const Foo *foo, int lev) {
  if (!foo) {
    return;
  }
  for (int i = 0; i < lev; i++) {
    std::cout << '\t';
  }
  foo->print();
  if (foo->left && foo->right) {
    printFoo(foo->left, lev + 1);
    printFoo(foo->right, lev + 1);
  }
}

int main(int argc, char **argv) {

  const std::string s =
      "Feature_ID:4,Threshold:11.000000,Description:Married-civ-spouse,Num_"
      "instances:1000,Loss:154.444294,Gain:33.554706,Num_constraints:0";
  Node nodeTest(s);

  //  std::ofstream myfile;
  //  myfile.open ()
  //  Foo *foo1 = new Foo(1.1);
  //  Foo *foo2 = new Foo(2.2);
  //  Foo *foo3 = new Foo(3.3, foo1, foo2);
  //  Foo *foo4 = new Foo(4.4);
  //  Foo *foo5 = new Foo(5.5);
  //  Foo *foo6 = new Foo(6.6, foo4, foo5);
  //  Foo *foo7 = new Foo(7.7, foo6, foo3);
  //  //foo7->print();
  //  printFoo(foo7, 0);
  //  delete foo7;
  //  std::cout << "Finish printing\n";

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

  //  // Build a dataset with pointers
  //  -------------------------------------------------------------------------
  //  const auto [rows, columnNames] =
  //  Dataset::getDatasetInfoFromFile(datasetFile); const unsigned cols =
  //  columnNames.size(); feature_t *X = (feature_t *)malloc(sizeof(feature_t) *
  //  rows * cols); label_t *y = (label_t *)malloc(sizeof(label_t) * rows);
  //  const auto [isNumerical, notNumericalEntries] =
  //  Dataset::fillXandYfromFile(X, rows, cols, y, datasetFile); std::cout <<
  //  "The notNumericalEntries size is:" << notNumericalEntries.size() <<
  //  std::endl;
  //
  //  Dataset dataset(X, rows, cols, y, utils::join(isNumerical, ','),
  //          utils::join(notNumericalEntries, ','),
  //          utils::join(columnNames, ','));
  //  std::cout << "The notNumericalEntries size is: " <<
  //  notNumericalEntries.size() << std::endl; std::cout << "Dataset:\n" <<
  //  dataset << std::endl;
  //
  //  // Free memory
  //  free((void *) X);
  //  free((void *) y);
  // //
  // --------------------------------------------------------------------------------------------------------

  const auto [rows, columnNames] = Dataset::getDatasetInfoFromFile(datasetFile);
  const unsigned cols = columnNames.size();
  feature_t *X = (feature_t *)malloc(sizeof(feature_t) * rows * cols);
  label_t *y = (label_t *)malloc(sizeof(label_t) * rows);
  const auto [isNumerical, notNumericalEntries] =
      Dataset::fillXandYfromFile(X, rows, cols, y, datasetFile);
  std::cout << "The notNumericalEntries size is:" << notNumericalEntries.size()
            << std::endl;

  Dataset dataset(X, rows, cols, y, utils::join(isNumerical, ','),
                  utils::join(notNumericalEntries, ','),
                  utils::join(columnNames, ','));
  //  Dataset dataset(datasetFile);
  std::cout << dataset << std::endl << std::endl;
  std::cout << "The dataset size is:" << dataset.size() << std::endl;
  DecisionTree dt(maxDepth);
  const bool useICML2019 = false;
  //  const bool useICML2019 = true;

  {
    const auto start = std::chrono::steady_clock::now();
    dt.fit(dataset, attackerFile, budget, threads, useICML2019, Impurity::SSE);
    const auto end = std::chrono::steady_clock::now();

    std::cout << "The decision tree is:" << std::endl << dt << std::endl;

    std::cout << "Time elapsed to fit the decision tree: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       start)
                     .count()
              << " milliseconds." << std::endl;
  }

  std::cout << "Is the decision tree trained? " << dt.isTrained() << std::endl;

  const std::string file = "example.txt";
  std::ofstream fs;
  fs.open(file, std::ios::out | std::ios::trunc);
  if (fs.is_open() && fs.good()) {
    fs << dt;
  }
  fs.close();

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
  dt.predict(X_test, rows, cols, predictions, false);
  std::cout << "Predictions:" << std::endl;
  std::cout << static_cast<int>(predictions[0]);
  for (index_t i = 1; i < rows; i++) {
    std::cout << "," << static_cast<int>(predictions[i]);
  }
  std::cout << std::endl;

  // Free memory
  free((void *)X);
  free((void *)y);
  free((void *)X_test);

  //  //
  //  ---------------------------------------------------------------------------
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
  //  std::string isNumerical = dataset.isFeatureNumerical(0) ? "True" :
  //  "False"; for (unsigned j = 1; j < cols; j++) {
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
  //    std::cout << "The decision tree is:" << std::endl << dt_copy <<
  //    std::endl;
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
