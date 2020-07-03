#include "cpp_implementation.hpp"

// External files
#include "Dataset.h"
// STL
#include <cassert>
#include <iostream>  // see pretty_print


PyDecisionTree::PyDecisionTree() :
    decisionTree_(DecisionTree()) , is_initialized(true) {
}

PyDecisionTree::~PyDecisionTree() {
  assert(is_initialized);
  is_initialized = false;
}

void PyDecisionTree::predict(const double *X, const unsigned rows, const unsigned cols, double *predictions, const bool score, const bool isRowsWise) const {
  assert(is_initialized);
  decisionTree_.predict(X, rows, cols, predictions, score, isRowsWise);
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
                         const bool useICML2019,
                         const unsigned int maxDepth,
                         const unsigned int minPerNode,
                         const bool isAffine) {
  assert(is_initialized);

  Dataset ds(X, rows, cols, y, isNumerical, notNumericalEntries, columnNames);
  decisionTree_.fit(ds, attackerFile, budget, threads, useICML2019, maxDepth, minPerNode, isAffine);
}

bool PyDecisionTree::is_trained() const {
  return decisionTree_.isTrained();
}

void PyDecisionTree::load(const char *filePath) {
  return decisionTree_.load(filePath);
}

void PyDecisionTree::save(const char *filePath) const {
  return decisionTree_.save(filePath);
}

void PyDecisionTree::pretty_print() const {
  std::cout << decisionTree_ << std::endl;
}
