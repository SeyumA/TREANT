//
// Created by dg on 14/01/20.
//

#ifndef TREEANT_SPLITOPTIMIZER_H
#define TREEANT_SPLITOPTIMIZER_H

#include "Attacker.h"
#include "Dataset.h"
#include <functional>
#include <tuple>
#include <vector>

class SplitOptimizer {

public:
  enum class Impurity { GINI, SSE, MSE, ENTROPY };

  explicit SplitOptimizer(Impurity impurityType);

  // It returns the best splitting feature and the best splitting value
  [[nodiscard]] std::tuple<gain_t, index_t, feature_t, indexes_t, indexes_t>
  optimizeGain(const Dataset &dataset, const indexes_t &validInstances,
               const indexes_t &validFeatures, const Attacker &attacker,
               const std::vector<cost_t> &costs) const;

private:
  // Private members -----------------------------------------------------------
  std::function<double(const Dataset &, const indexes_t &, bool)> getLoss_;

  // Functions -----------------------------------------------------------------
  // Called by the optimizeGain, this function solves the optimization problem.
  // (see __optimize_sse_under_max_attack in the python code
  // It returns y_pred_left, y_pred_right, sse_uma
  [[nodiscard]] bool optimizeSSE(const Dataset &dataset,
                                 const indexes_t &validInstances,
                                 const split_t &bestSplit, double &yHatLeft,
                                 double &yHatRight, double &sse
                                 /*TODO: lossFunction, constraints*/) const;

  // Returns left, unknown, right
  void simulateSplit(const Dataset &dataset, const indexes_t &validInstances,
                     const indexes_t &validFeatures, const Attacker &attacker,
                     const std::vector<cost_t> &costs, feature_t splittingValue,
                     index_t splittingFeature, feature_vector_t &leftSplit,
                     feature_vector_t &rightSplit,
                     feature_vector_t &unknownSplit) const;

  static bool getDefaultPrediction(const Dataset &dataset,
                                   const indexes_t &validInstances);
};

#endif // TREEANT_SPLITOPTIMIZER_H
