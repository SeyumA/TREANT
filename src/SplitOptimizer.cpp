//
// Created by dg on 14/01/20.
//

#include "SplitOptimizer.h"
#include <cassert>
#include <stdexcept>

SplitOptimizer::SplitOptimizer(SplitOptimizer::Impurity impurityType) {

  switch (impurityType) {
  case Impurity::GINI:
    throw std::runtime_error("Implement GINI");
  case Impurity::SSE:
    getLoss_ = [](const Dataset &dataset, const indexes_t &validInstances,
                  bool yPred) {
      double sum = 0.0;
      const auto &labels = dataset.getLabels();
      for (const auto &i : validInstances) {
        double diff = labels[i] == yPred ? 0.0 : 1.0;
        sum += diff * diff;
      }
      return sum;
    };
  case Impurity::MSE:
    throw std::runtime_error("Implement MSE");
  case Impurity::ENTROPY:
    throw std::runtime_error("Implement ENTROPY");
  default:
    throw std::runtime_error("Not valid impurity type");
  }
}

void SplitOptimizer::simulateSplit(
    const Dataset &dataset, const indexes_t &validInstances,
    const indexes_t &validFeatures, const Attacker &attacker,
    const std::vector<int> &costs, feature_t splittingValue,
    index_t splittingFeature, feature_vector_t &leftSplit,
    feature_vector_t &rightSplit, feature_vector_t &unknownSplit) const {





}

bool SplitOptimizer::optimizeSSE(const Dataset &dataset,
                                 const indexes_t &validInstances,
                                 const split_t &bestSplit, double &yHatLeft,
                                 double &yHatRight, double &sse) const {

  // TODO implement this also with static
  //     Use the external library

  // This function returns the 2 best thresholds (left and right) for the

  if (dataset.empty()) {
    return false;
  }

  return false;
}

std::tuple<gain_t, index_t, feature_t, indexes_t, indexes_t>
SplitOptimizer::optimizeGain(const Dataset &dataset,
                             const indexes_t &validInstances,
                             const indexes_t &validFeatures,
                             const Attacker &attacker,
                             const std::vector<cost_t> &costs) const {

  // In general, assert diagnoses an error in the implementation: at this point
  // the validInstances vector can not be empty.
  assert(!validInstances.empty());

  // Build a map [feature_id, possible feature choices] that includes only the
  // feature still available (not already used, so not in the blacklist)

  // Here we just make a double nested loop on instances and features and then
  // we call optimizeSSE For each valid feature and for each valid
  // instance

  // Get the current prediction that is just the mean value of the labels
  // of the subset instances
  const bool currPrediction =
      SplitOptimizer::getDefaultPrediction(dataset, validInstances);

  // Get the default score
  // It is calculated with evaluate_split in the python code
  // with self.split_optimizer.evaluate_split (line 1521) ->
  // usually SplitOptimizer._SplitOptimizer__sse; could be also logloss
  // gini, ... so we put it in the getLoss_ function
  double currScore = getLoss_(dataset, validInstances, currPrediction);

  // Default value of the gain, so how much we can improve the current score.
  gain_t bestGain = 0.0;

  for (const auto &validFeatureIndex : validFeatures) {
    const auto &featureVector = dataset.getFeatureColumn(validFeatureIndex);
    for (const auto &validInstance : validInstances) {
      // Take the candidate splitting value among the valid instances
      // TODO: use quantiles as splitting values
      feature_t splittingValue = featureVector[validInstance];
      // find the best split with this value
      // line 1169 of the python code it is called self.__simulate_split
      const auto currentSplit =
          simulateSplit(dataset, validInstances, validFeatures, attacker, costs,
                        splittingValue, validFeatureIndex);
      //
      // TODO: propagate the constraints (see lines 1177-1190)
      //

      double yHatLeft, yHatRight, sse;
      bool optSuccess = optimizeSSE(dataset, validInstances, currentSplit,
                                    yHatLeft, yHatRight, sse);

      if (optSuccess) {
        const double currGain = currScore - sse;
        if (currGain > bestGain) {
          bestGain = currGain;

        }
      }
      // TODO: continue from here

      //
    }
  }

  return {0.0, 0, 1.5, std::vector<index_t>(), std::vector<index_t>()};
}
bool SplitOptimizer::getDefaultPrediction(const Dataset &dataset,
                                          const indexes_t &validInstances) {
  const auto &labels = dataset.getLabels();
  double sumLabels = 0.0;
  for (const auto &validInstance : validInstances) {
    double yAsDouble = labels[validInstance] ? 1.0 : 0.0;
    sumLabels += yAsDouble;
  }
  const double mean = sumLabels / static_cast<double>(validInstances.size());
  return mean > 0.5;
}
