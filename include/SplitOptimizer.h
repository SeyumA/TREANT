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
  // The costs are maps because are always subsets
  [[nodiscard]] bool
  optimizeGain(const Dataset &dataset, const indexes_t &validInstances,
               const indexes_t &validFeatures, const Attacker &attacker,
               const std::unordered_map<index_t, cost_t> &costs,
               const std::vector<Constraint> &constraints,
               const double &currentScore, const double &currentPredictionScore,
               const unsigned &numThreads, const bool &useICML2019,
               // outputs
               gain_t &bestGain, indexes_t &bestSplitLeft,
               indexes_t &bestSplitRight, index_t &bestSplitFeatureId,
               split_value_t &bestSplitValue, split_value_t &bestNextSplitValue,
               prediction_t &bestPredLeft, prediction_t &bestPredRight,
               double &bestSSEuma, std::vector<Constraint> &constraintsLeft,
               std::vector<Constraint> &constraintsRight,
               std::unordered_map<index_t, cost_t> &costsLeft,
               std::unordered_map<index_t, cost_t> &costsRight
               // TODO: put the outputs in a struct
  ) const;

  [[nodiscard]] double evaluateSplit(const Dataset &dataset,
                                     const indexes_t &validFeatures,
                                     prediction_t prediction) const;

private:
  // Private members -----------------------------------------------------------
  std::function<double(const Dataset &, const indexes_t &, label_t)> getLoss_;

};

#endif // TREEANT_SPLITOPTIMIZER_H
