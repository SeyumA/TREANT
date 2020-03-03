//
// Created by dg on 14/01/20.
//

#include <cassert>
#include <set>
#include <memory>
#include <stdexcept>

#include "SplitOptimizer.h"

// NLOPT test
#include <cmath>
#include <nlopt.hpp>
// TODO: delete this include (only for debug purposes)
#include <iomanip>
#include <iostream>

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
                                   const feature_t &splittingValue,
                                   const index_t &splittingFeature,
                                   // outputs
                                   indexes_t &leftSplit, indexes_t &rightSplit,
                                   indexes_t &unknownSplit) const {

  // Prepare the output
  if (!(leftSplit.empty() && rightSplit.empty() && unknownSplit.empty())) {
    throw std::runtime_error(
        "ERROR in simulateSplit: all outputs must be empty at the beginning");
  }

  bool isNumerical = dataset.isFeatureNumerical(splittingFeature);
  const auto &featureColumn = dataset.getFeatureColumn(splittingFeature);

  for (const auto &i : validInstances) {
    const auto cost = costs[i];
    // The attack on a specific instance 'i' to its feature 'splittingFeature'
    // generates a set of new feature,
    // of those we are interested only in the i-th column

    // See line 1014 of parallel_robust_forest.py
    bool allLeft = true;
    bool allRight = true;

    const std::string featureName = dataset.getFeatureName(splittingFeature);
    const auto attackedFeatureValue = featureColumn[i];
    std::vector<feature_t> attacks =
        attacker.attack(featureName, attackedFeatureValue, cost);
    for (const auto &atk : attacks) {
      if (isNumerical) {
        if (atk <= splittingValue) {
          allRight = false;
        } else {
          allLeft = false;
        }
        if (!allLeft && !allRight) {
          break;
        }
      } else {
        if (atk == splittingValue) {
          allRight = false;
        } else {
          allLeft = false;
        }
        if (!allLeft && !allRight) {
          break;
        }
      }
    }
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

// Start Nlopt library test
typedef struct {
  double a, b;
} my_constraint_data;

struct ExtraData {
  explicit ExtraData(const std::vector<label_t> &y,
                     const indexes_t &leftIndexes,
                     const indexes_t &rightIndexes,
                     const indexes_t &unknownIndexes)
      : y_(y), leftIndexes_(leftIndexes), rightIndexes_(rightIndexes),
        unknownIndexes_(unknownIndexes) {}
  const std::vector<label_t> &y_;
  const indexes_t &leftIndexes_;
  const indexes_t &rightIndexes_;
  const indexes_t &unknownIndexes_;
};

int count = 0;

double myvfunc(const std::vector<double> &x, std::vector<double> &grad,
               void *my_func_data) {

  static const auto updateRetAndGrad =
      [&grad](const double &diff, const index_t &gradIndex, double &ret) {
        if (!grad.empty()) {
          grad[gradIndex] = 2.0 * diff;
        }
        ret += diff * diff;
      };

  // Cast the extra data
  const auto *d = reinterpret_cast<ExtraData *>(my_func_data);
  const auto leftIndexesPtr = d->leftIndexes_.data();
  // Update the iteration count
  ++count;
  std::cout << "iteration number " << count << std::endl;
  // Initialize the returning value
  const auto &y = d->y_;
  double ret = 0.0;
  double sumDiff0 = 0.0;
  double sumDiff1 = 0.0;
  // Working on the left indexes
  for (const auto &leftIndex : d->leftIndexes_) {
    const double diff = x[0] - y[leftIndex];
    sumDiff0 += diff;
    ret += diff * diff;
  }
  // Working on the right indexes
  int i = 0;
  for (const auto &rightIndex : d->rightIndexes_) {
    const double diff = x[1] - y[rightIndex];
    sumDiff1 += diff;
    ret += diff * diff;
  }
  // Working on the unknown indexes
  for (const auto &unknownIndex : d->unknownIndexes_) {
    const double diffLeft = x[0] - y[unknownIndex];
    const double diffLeftAbs = diffLeft < 0.0 ? -diffLeft : diffLeft;
    const double diffRight = y[unknownIndex] - x[1];
    const double diffRightAbs = diffRight < 0.0 ? -diffRight : diffRight;
    const double diff = diffLeftAbs > diffRightAbs ? diffLeft : diffRight;
    // diff is maximum possible difference, i.e. the difference between:
    // |y[unknownIndex] - leftPred| and |y[unknownIndex] - rightPred|
    if (diffLeftAbs > diffRightAbs) {
      sumDiff0 += diff;
    } else {
      sumDiff1 += diff;
    }
    ret += diff * diff;
  }

  // Update the gradient
  if (!grad.empty()) {
    grad[0] = 2.0 * sumDiff0;
    grad[1] = 2.0 * sumDiff1;
  }

  //
  return ret;
}

//double myvconstraint(const std::vector<double> &x, std::vector<double> &grad,
//                     void *data) {
//  auto *d = reinterpret_cast<my_constraint_data *>(data);
//  double a = d->a, b = d->b;
//  if (!grad.empty()) {
//    grad[0] = 3 * a * (a * x[0] + b) * (a * x[0] + b);
//    grad[1] = -1.0;
//  }
//  return ((a * x[0] + b) * (a * x[0] + b) * (a * x[0] + b) - x[1]);
//}

// NLOPT test end

bool SplitOptimizer::optimizeSSE(const std::vector<label_t> &y,
                                 const indexes_t &leftSplit,
                                 const indexes_t &rightSplit,
                                 const indexes_t &unknownSplit,
                                 label_t &yHatLeft, label_t &yHatRight,
                                 gain_t &sse) const {

  // TODO implement this also with static
  //     Use the external library

  // The dimension of the problem is 2: we are finding yHatLeft and yHatRight
  nlopt::opt opt(nlopt::LD_SLSQP, 2);
  const auto extraDataPtr = std::make_shared<ExtraData>(y, leftSplit, rightSplit, unknownSplit);
  // TODO: myvfunc must be a static function of SplitOptimizer
  opt.set_min_objective(myvfunc, extraDataPtr.get());
  // Set the tolerance
  opt.set_xtol_rel(1e-4);
  // Initialize the x vector
  std::vector<feature_t> x = {yHatLeft, yHatRight};

  double minf = 0.0;

  try {
    nlopt::result result = opt.optimize(x, minf);
    if (result < nlopt::result::SUCCESS) {
      std::cout << "The result is not SUCCESS";
      return false;
    } else {
      printf("found minimum after %d evaluations\n", count);
      std::cout << "found minimum at f(" << x[0] << "," << x[1]
                << ") = " << std::setprecision(10) << minf << std::endl;
      // Update the outputs
      yHatLeft = x[0];
      yHatRight = x[1];
      sse = minf;
      return true;
    }
  } catch (std::exception &e) {
    std::cout << "nlopt failed: " << e.what() << std::endl;
    return false;
  }
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
    // TODO: uniqueFeatureValues Could be cached
    const auto &currentColumn = dataset.getFeatureColumn(splittingFeature);
    const std::set<feature_t> uniqueFeatureValues(currentColumn.begin(),
                                                  currentColumn.end());

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

      feature_t yHatLeft = currentPredictionScore;
      feature_t yHatRight = currentPredictionScore;
      feature_t sse = 0.0;
      bool optSuccess = optimizeSSE(dataset.getLabels(), leftSplit, rightSplit,
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
