#pragma once

#include "DecisionTree.h"

class PyDecisionTree {
public:
  PyDecisionTree(unsigned int maxDepth);
  ~PyDecisionTree();

  void predict(const double *X, const unsigned rows, const unsigned cols, double *predictions) const;

  void fit(const double *X,
           const unsigned rows,
           const unsigned cols,
           const double *y,
           const char *isNumerical,
           const char *notNumericalEntries,
           const char *columnNames,
           const char *attackerFile,
           const double budget,
           const unsigned threads,
           const bool useICML2019);

  bool is_trained() const;

  void pretty_print() const;

private:

  DecisionTree decisionTree_;
  bool is_initialized;
};
