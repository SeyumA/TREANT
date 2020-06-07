#pragma once

#include "DecisionTree.h"

class PyDecisionTree {
public:
  PyDecisionTree(unsigned int maxDepth);
  ~PyDecisionTree();

  bool predict() const;
  void fit(const char *datasetFile,
           const char *attackerFile,
           const double budget,
           const unsigned threads);
  bool is_trained() const;
  void pretty_print() const;
  
private:

  int maxDepth_;
  DecisionTree decisionTree_;

  bool is_initialized;
};
