#include "cpp_implementation.hpp"

// External files
#include "Dataset.h"

#include <cassert>
//Just for logging
#include <iostream>

PyDecisionTree::PyDecisionTree(unsigned int maxDepth) :
    maxDepth_(maxDepth), decisionTree_(DecisionTree(maxDepth, false)) {
  is_initialized = true;
}

PyDecisionTree::~PyDecisionTree() {
  assert(is_initialized);
  is_initialized = false;
}

bool PyDecisionTree::predict() const {
  assert(is_initialized);
  return false;
}

void PyDecisionTree::fit(const char *datasetFile,
                         const char *attackerFile,
                         const double budget,
                         const unsigned threads, const bool useICML2019) {
  assert(is_initialized);
  
  Dataset ds(datasetFile);
  decisionTree_.fit(ds, attackerFile, budget, threads, useICML2019);
}

bool PyDecisionTree::is_trained() const {
  return decisionTree_.isTrained();
}

void PyDecisionTree::pretty_print() const {
  std::cout << decisionTree_ << std::endl;
}
