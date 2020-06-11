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
               const unsigned &numThreads,
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

  // Functions -----------------------------------------------------------------
  // Called by the optimizeGain, this function solves the optimization problem.
  // (see __optimize_sse_under_max_attack in the python code
  // It returns y_pred_left, y_pred_right, sse_uma
  [[nodiscard]] bool
  optimizeSSE(const std::vector<label_t> &y, const indexes_t &leftSplit,
              const indexes_t &rightSplit, const indexes_t &unknownSplit,
              std::vector<Constraint> &constraints, label_t &yHatLeft,
              label_t &yHatRight, gain_t &sse
              /*TODO: lossFunction, constraints*/) const;

  // Returns left, unknown, right
  [[nodiscard]] std::tuple<indexes_t, indexes_t, indexes_t>
  simulateSplit(const Dataset &dataset, const indexes_t &validInstances,
                const Attacker &attacker,
                const std::unordered_map<index_t, cost_t> &costs,
                const feature_t &splittingValue,
                const index_t &splittingFeature) const;

  [[nodiscard]] std::tuple<indexes_t, indexes_t, indexes_t, bool>
  simulateSplitICML2019(const Dataset &dataset, const indexes_t &validInstances,
                        const Attacker &attacker,
                        const std::unordered_map<index_t, cost_t> &costs,
                        const feature_t &splittingValue,
                        const index_t &splittingFeature, label_t &yHatLeft,
                        label_t &yHatRight, gain_t &sse) const;

  [[nodiscard]] double sseICML2019(const indexes_t &firstPart,
                                   const indexes_t &secondPart,
                                   const std::vector<prediction_t> &yTrue,
                                   const prediction_t &yPred) const;

  [[nodiscard]] std::optional<std::tuple<label_t, label_t, gain_t>>
  lossICML2019(const indexes_t &icmlLeftFirst, const indexes_t &icmlLeftSecond,
               const indexes_t &icmlRightFirst,
               const indexes_t &icmlRightSecond,
               const std::vector<prediction_t> &y) const;

  // Move the implementation to SplitOptimizer.cpp
  struct ExtraData {
    explicit ExtraData(const std::vector<label_t> &y,
                       const indexes_t &leftIndexes,
                       const indexes_t &rightIndexes,
                       const indexes_t &unknownIndexes);

    const std::vector<label_t> &y_;
    const indexes_t &leftIndexes_;
    const indexes_t &rightIndexes_;
    const indexes_t &unknownIndexes_;
    std::size_t count_;
  };

  static double sseCostFunction(const std::vector<double> &x,
                                std::vector<double> &grad, void *data);

  static double constraintFunction(const std::vector<double> &x,
                                   std::vector<double> &grad, void *data);
};

#endif // TREEANT_SPLITOPTIMIZER_H
