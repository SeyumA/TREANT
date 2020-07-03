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
  DecisionTree dt;
  //
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
