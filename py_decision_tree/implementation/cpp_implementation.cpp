#include "cpp_implementation.hpp"

// External files
#include "SplitOptimizer.h"

#include <cassert>
//Just for logging
#include <iostream>

PyDecisionTree::PyDecisionTree(const char *datasetFile) : dataset_(Dataset(datasetFile)){
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
  
  std::cout << "The dataset owned is:" << std::endl << dataset_ << "\n\n";
  
  SplitOptimizer splitOptimizer;
  std::cout << "TODO: Using nlopt:" << std::endl;
  //splitOptimizer.perform();
}
