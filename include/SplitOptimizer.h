//
// Created by dg on 14/01/20.
//

#ifndef TREEANT_SPLITOPTIMIZER_H
#define TREEANT_SPLITOPTIMIZER_H

#include <functional>
#include <tuple>
#include <vector>

#include "Attacker.h"
#include "Constraint.h"
#include "Dataset.h"
#include "types.h"

class SplitOptimizer {

public:
  explicit SplitOptimizer(Impurity impurityType);

  // It returns the best splitting feature and the best splitting value
  [[nodiscard]] bool
  optimizeGain(const Dataset &dataset, const indexes_t &validInstances,
               const indexes_t &validFeatures, const Attacker &attacker,
               const std::vector<cost_t> &costs,
               const std::vector<Constraint> &constraints,
               const double &currentScore, const double &currentPredictionScore,
               // outputs
               gain_t &bestGain, indexes_t &bestSplitLeftFeatureId,
               indexes_t &bestSplitRightFeatureId, index_t &bestSplitFeatureId,
               split_value_t &bestSplitValue, split_value_t &bestNextSplitValue,
               prediction_t &bestPredLeft, prediction_t &bestPredRight,
               double &bestSSEuma, std::vector<Constraint> &constraintsLeft,
               std::vector<Constraint> &constraintsRight,
               std::vector<cost_t> &costsLeft, std::vector<cost_t> &costsRight
               // TODO: put the outputs in a struct
  ) const;

  [[nodiscard]] double evaluateSplit(const Dataset &dataset,
                                     const indexes_t &validFeatures,
                                     prediction_t prediction) const;

private:
  // Private members -----------------------------------------------------------
  std::function<double(const Dataset &, const indexes_t &, label_t)> getLoss_;

  // Functions -----------------------------------------------------------------
  // Called by the optimizeGain, this function solves the optimization problem.
  // (see __optimize_sse_under_max_attack in the python code
  // It returns y_pred_left, y_pred_right, sse_uma
  [[nodiscard]] bool
  optimizeSSE(const Dataset &dataset, const indexes_t &validInstances,
              const indexes_t &leftSplit, const indexes_t &rightSplit,
              const indexes_t &unknownSplit, double &yHatLeft,
              double &yHatRight, double &sse
              /*TODO: lossFunction, constraints*/) const;

  // Returns left, unknown, right
  // TODO: this code can go easily a method in the FeatureColumn class
  void simulateSplit(const Dataset &dataset, const indexes_t &validInstances,
                     const Attacker &attacker, const std::vector<cost_t> &costs,
                     const generic_feature_t &splittingValue,
                     const index_t &splittingFeature, indexes_t &leftSplit,
                     indexes_t &rightSplit, indexes_t &unknownSplit) const;
};

#endif // TREEANT_SPLITOPTIMIZER_H