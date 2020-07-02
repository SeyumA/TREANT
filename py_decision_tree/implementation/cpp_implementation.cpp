#include "cpp_implementation.hpp"

// External files
#include "Dataset.h"

#include <cassert>
//Just for logging
#include <iostream>

PyDecisionTree::PyDecisionTree(unsigned int maxDepth) :
    decisionTree_(DecisionTree(maxDepth, false)) , is_initialized(true) {
}

PyDecisionTree::~PyDecisionTree() {
  assert(is_initialized);
  is_initialized = false;
}

void PyDecisionTree::predict(const double *X, const unsigned rows, const unsigned cols, double *predictions) const {
  assert(is_initialized);
  decisionTree_.predict(X, rows, cols, predictions);
}

void PyDecisionTree::fit(const double *X,
                         const unsigned rows,
                         const unsigned cols,
                         const double *y,
                         const char *isNumerical,
                         const char *notNumericalEntries,
                         const char *columnNames,
                         const char *attackerFile,
                         const double budget,
                         const unsigned threads,
                         const bool useICML2019) {
  assert(is_initialized);

  Dataset ds(X, rows, cols, y, isNumerical, notNumericalEntries, columnNames);
  decisionTree_.fit(ds, attackerFile, budget, threads, useICML2019);
}

bool PyDecisionTree::is_trained() const {
  return decisionTree_.isTrained();
}

void PyDecisionTree::pretty_print() const {
  std::cout << decisionTree_ << std::endl;
}
