#include "cpp_implementation.hpp"

// External files
#include "DecisionTree.h"

#include <cassert>
// Just for logging
#include <iostream>

PyDecisionTree::PyDecisionTree(const char *datasetFile)
    : dataset_(Dataset(datasetFile)) {
  balance = 0.0;

  // Hardcoded for now
  maxDepth_ = 4;

  is_initialized = true;
}

PyDecisionTree::~PyDecisionTree() {
  assert(is_initialized);
  is_initialized = false;
}

bool PyDecisionTree::predict() const {
  assert(is_initialized);
  return vec.size() > 0;
}

void PyDecisionTree::fit() {
  assert(is_initialized);
  // working with std::vector
  // vec.resize(10);

  // Hardcoded, must be moved as arguments
  const std::string attackerFile =
      "/home/dg/source/repos/uni/treeant/data/attacks.json";
  int budget = 0;
  int threads = 1;

  // std::cout << "The dataset owned is:" << std::endl << dataset_ << "\n\n";

  DecisionTree dt(maxDepth_);

  std::cout << "Fitting a DecisionTree. TODO: save the root pointer\n";
  dt.fit(dataset_, attackerFile, budget, threads);
  std::cout << "The decision tree is:\n" << dt << std::endl;
}
