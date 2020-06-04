#pragma once

#include "Dataset.h"
#include <vector>

class PyDecisionTree {
public:
  PyDecisionTree(const char *datasetFile);
  ~PyDecisionTree();

  bool predict() const;
  void fit();

private:

  Dataset dataset_;
  int maxDepth_;

  std::vector<int> vec;
  double balance;
  bool is_initialized;
};
