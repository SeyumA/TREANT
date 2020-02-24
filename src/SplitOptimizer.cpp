//
// Created by dg on 14/01/20.
//

#include "SplitOptimizer.h"
#include <cassert>
#include <stdexcept>

SplitOptimizer::SplitOptimizer(Impurity impurityType) {

  switch (impurityType) {
  case Impurity::GINI:
    throw std::runtime_error("Implement GINI");
    break;
  case Impurity::SSE:
    getLoss_ = [](const Dataset &dataset, const indexes_t &validInstances,
                  label_t yPred) {
      double sum = 0.0;
      const auto &labels = dataset.getLabels();
      for (const auto &i : validInstances) {
        const double diff = labels[i] - yPred;
        sum += diff * diff;
      }
      return sum;
    };
    break;
  case Impurity::MSE:
    throw std::runtime_error("Implement MSE");
    break;
  case Impurity::ENTROPY:
    throw std::runtime_error("Implement ENTROPY");
    break;
  default:
    throw std::runtime_error("Not valid impurity type");
    break;
  }
}

void SplitOptimizer::simulateSplit(const Dataset &dataset,
                                   const indexes_t &validInstances,
                                   const Attacker &attacker,
                                   const std::vector<cost_t> &costs,
                                   const generic_feature_t &splittingValue,
                                   const index_t &splittingFeature,
                                   // outputs
                                   indexes_t &leftSplit,
                                   indexes_t &rightSplit,
                                   indexes_t &unknownSplit) const {

  // Prepare the output
  if (!(leftSplit.empty() && rightSplit.empty() && unknownSplit.empty())) {
    throw std::runtime_error(
        "ERROR in simulateSplit: all outputs must be empty at the beginning");
  }

  bool isNumerical = dataset.getFeatureColumn(splittingFeature).isNumerical();
  const auto vPtrAsFp = std::get_if<fp_feature_t>(&splittingValue);
  const auto vPtrAsCt = std::get_if<ct_feature_t>(&splittingValue);

  if (!( (isNumerical && vPtrAsFp) || (!isNumerical && vPtrAsCt) )) {
    throw std::runtime_error(
        "ERROR in simulateSplit: splittingValue NOT consistent");
  }

  for (const auto& i : validInstances) {
    const auto cost = costs[i];
    // The attack on a specific instance 'i' to its feature 'splittingFeature'
    // generates a set of new feature,
    // of those we are interested only in the i-th column

    // See line 1014 of parallel_robust_forest.py
    bool allLeft = true;
    bool allRight = true;

    const std::string featureName = dataset.getFeatureName(splittingFeature);
    if (isNumerical) {
      const fp_feature_t v = std::get<fp_feature_t>(splittingValue);
      std::vector<fp_feature_t> attacks = attacker.attack(featureName, v, cost);
      for (const auto& atk : attacks) {
        if (atk <= v) {
          allRight = false;
        } else {
          allLeft = false;
        }
        if (!allLeft && !allRight) {
          break;
        }
      }
    } else {
      const ct_feature_t v = std::get<ct_feature_t>(splittingValue);
      std::vector<ct_feature_t> attacks = attacker.attack(featureName, v, cost);
      for (const auto& atk : attacks) {
        if (atk == v) {
          allRight = false;
        } else {
          allLeft = false;
        }
        if (!allLeft && !allRight) {
          break;
        }
      }
    } // end if isNumerical
    // Modify the output vectors accordingly
    if (allLeft) {
      leftSplit.push_back(i);
    } else if (allRight) {
      rightSplit.push_back(i);
    } else {
      unknownSplit.push_back(i);
    }
  } // end of loop over instances
}

bool SplitOptimizer::optimizeSSE(const Dataset &dataset,
                                 const indexes_t &validInstances,
                                 const indexes_t &leftSplit,
                                 const indexes_t &rightSplit,
                                 const indexes_t &unknownSplit,
                                 double &yHatLeft, double &yHatRight,
                                 double &sse) const {

  // TODO implement this also with static
  //     Use the external library
  throw std::runtime_error("optimizeSSE is not implemented");
}

bool SplitOptimizer::optimizeGain(
    const Dataset &dataset, const indexes_t &validInstances,
    const indexes_t &validFeatures, const Attacker &attacker,
    const std::vector<cost_t> &costs,
    const std::vector<Constraint> &constraints, const double &currentScore,
    const double &currentPredictionScore, // (used by)/(forward to) optimizeSSE
    // outputs
    // TODO: better put this in 2 structs (one left, one right) and return it
    gain_t &bestGain, indexes_t &bestSplitLeftFeatureId,
    indexes_t &bestSplitRightFeatureId, index_t &bestSplitFeatureId,
    split_value_t &bestSplitValue, split_value_t &bestNextSplitValue,
    prediction_t &bestPredLeft, prediction_t &bestPredRight, double &bestSSEuma,
    std::vector<Constraint> &constraintsLeft,
    std::vector<Constraint> &constraintsRight, std::vector<cost_t> &costsLeft,
    std::vector<cost_t> &costsRight) const {

  // In general, assert diagnoses an error in the implementation: at this point
  // the validInstances vector can not be empty.
  assert(!validInstances.empty());

  // Default value of the gain, returns how much we can improve the current
  // score.
  bestGain = 0.0;

  for (const auto &splittingFeature : validFeatures) {

    // Build a set of unique feature values
    // TODO: Could be cached but optimizeGain is called only once in
    // fitRecursively
    const auto currentColumn = dataset.getFeatureColumn(splittingFeature);
    const auto uniqueFeatureValues = currentColumn.getUniqueValues();

    for (const auto &splittingValue : uniqueFeatureValues) {
      // Take the candidate splitting value among the valid instances
      // TODO: use quantiles as splitting values

      // find the best split with this value
      // line 1169 of the python code it is called self.__simulate_split
      indexes_t leftSplit, rightSplit, unknownSplit;
      simulateSplit(dataset, validInstances, attacker, costs, splittingValue,
                    splittingFeature, leftSplit, rightSplit, unknownSplit);
      //
      // TODO: propagate the constraints (see lines 1177-1190)
      //

      double yHatLeft, yHatRight, sse;
      bool optSuccess =
          optimizeSSE(dataset, validInstances, leftSplit, rightSplit,
                      unknownSplit, yHatLeft, yHatRight, sse);

      if (optSuccess) {
        const double currGain = currentScore - sse;
        if (currGain > bestGain) {
          bestGain = currGain;
        }
      }
      // TODO: continue from here

      //
    }
  }

  return false;
}

double SplitOptimizer::evaluateSplit(const Dataset &dataset,
                                     const indexes_t &rows,
                                     prediction_t prediction) const {
  return getLoss_(dataset, rows, prediction);
}
