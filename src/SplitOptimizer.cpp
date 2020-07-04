#include <cassert>
#include <future>
#include <set>
#include <stdexcept>

//#include <omp.h>

#include "SplitOptimizer.h"
#include "utils.h"
//// NLOPT test
//#include <cmath>
//#include <nlopt.hpp>
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
      for (const auto &i : validInstances) {
        const double diff = dataset(i) - yPred;
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

bool SplitOptimizer::optimizeGain(
    const Dataset &dataset, const indexes_t &validInstances,
    const indexes_t &validFeatures, const Attacker &attacker,
    const std::unordered_map<index_t, cost_t> &costs,
    const std::vector<Constraint> &constraints, const double &currentScore,
    const double &currentPredictionScore, // (used by)/(forward to) optimizeSSE
    const unsigned &numThreads, const bool &useICML2019,
    // outputs
    // TODO: better put this in 2 structs (one left, one right) and return it
    gain_t &bestGain, indexes_t &bestSplitLeft, indexes_t &bestSplitRight,
    index_t &bestSplitFeatureId, split_value_t &bestSplitValue,
    split_value_t &bestNextSplitValue, prediction_t &bestPredLeft,
    prediction_t &bestPredRight, double &bestSSEuma,
    std::vector<Constraint> &constraintsLeft,
    std::vector<Constraint> &constraintsRight,
    std::unordered_map<index_t, cost_t> &costsLeft,
    std::unordered_map<index_t, cost_t> &costsRight) const {

  // In general, assert diagnoses an error in the implementation: at this point
  // the validInstances vector can not be empty.
  assert(!validInstances.empty());

  // Split in subsets the features in order to process them in different subsets
  const std::vector<std::vector<index_t>> batches =
      utils::buildBatches(numThreads, validFeatures);

//  // Measure time
//  const auto start = std::chrono::steady_clock::now();

  // Default value of the gain, returns how much we can improve the current
  // score.
  bestGain = -1.0f;
  if (batches.size() == 1) {
    const auto singleThreadResult = utils::optimizeOnSubset(dataset, costs,
        attacker, batches[0], validInstances, constraints,
        currentScore, currentPredictionScore, useICML2019);
    if (singleThreadResult.bestGain > bestGain) {
      bestGain = singleThreadResult.bestGain;
      bestSplitFeatureId = singleThreadResult.bestSplitFeatureId;
      bestSplitValue = singleThreadResult.bestSplitValue;
      bestNextSplitValue = singleThreadResult.bestNextSplitValue;
      bestPredLeft = singleThreadResult.bestPredLeft;
      bestPredRight = singleThreadResult.bestPredRight;
      bestSSEuma = singleThreadResult.bestSSEuma;
    } else {
      throw std::runtime_error("Optimizer cannot find the optimal feature");
    }
  } else {
    std::vector<std::future<utils::OptimizeOutput>> batchResults(numThreads);
    for (std::size_t i = 0; i < batches.size(); ++i) {
      batchResults[i] =
          std::async(std::launch::async, utils::optimizeOnSubset, dataset, costs,
                     attacker, batches[i], validInstances, constraints,
                     currentScore, currentPredictionScore, useICML2019);
    }

    // Reduction
    for (auto &fut : batchResults) {
      const auto result = fut.get();
      if (result.bestGain > bestGain) {
        bestGain = result.bestGain;
        bestSplitFeatureId = result.bestSplitFeatureId;
        bestSplitValue = result.bestSplitValue;
        bestNextSplitValue = result.bestNextSplitValue;
        bestPredLeft = result.bestPredLeft;
        bestPredRight = result.bestPredRight;
        bestSSEuma = result.bestSSEuma;
      }
    }
  }
//  const auto end = std::chrono::steady_clock::now();
//  std::cout << "Time elapsed in parallel SplitOptimer: "
//            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
//                                                                     start)
//                   .count()
//            << " milliseconds." << std::endl;

  // If the bestGain is > 0.0 than there is an improvement on the solution
  // otherwise we return false
  indexes_t bestSplitUnknown;
  if (bestGain > 0.0) {

    // Sanity check on constraints
    if (!(constraintsLeft.empty() && constraintsRight.empty())) {
      throw std::runtime_error(
          "left and right constraints must be empty at the beginning");
    }
    if (!(costsLeft.empty() && costsRight.empty())) {
      throw std::runtime_error(
          "left and right costs must be empty at the beginning");
    }

    if (useICML2019) {
      // Check on empty constraints based on ICML2019 strategy
      if (!constraints.empty()) {
        throw std::runtime_error(
            "Constraints are not considered in the ICML2019 strategy");
      }
      // Recover the best split
      label_t dummyL = 0.0;
      label_t dummyR = 0.0;
      gain_t dummySSE = 0.0;
      const auto [leftSplit, rightSplit, unknownSplit, success] =
          utils::simulateSplitICML2019(dataset, validInstances, attacker, costs,
                                       bestSplitValue, bestSplitFeatureId,
                                       dummyL, dummyR, dummySSE);
      bestSplitLeft = std::move(leftSplit);
      bestSplitRight = std::move(rightSplit);
      bestSplitUnknown = std::move(unknownSplit);
      assert(success);
      // Distribute the unknown indexes
      for (const auto &unknownIndex : bestSplitUnknown) {
        if (dataset(unknownIndex, bestSplitFeatureId) <= bestSplitValue) {
          bestSplitLeft.push_back(unknownIndex);
        } else {
          bestSplitRight.push_back(unknownIndex);
        }
      }

      // Update the left and right costs
      for (const auto &leftIndex : bestSplitLeft) {
        costsLeft[leftIndex] = costs.at(leftIndex);
      }
      for (const auto &rightIndex : bestSplitRight) {
        costsRight[rightIndex] = costs.at(rightIndex);
      }

    } else {
      // Recover the best split: bestSplitLeft, bestSplitRight, bestSplitUnknown
      {
        auto [leftSplit, rightSplit, unknownSplit] =
            utils::simulateSplit(dataset, validInstances, attacker, costs,
                                 bestSplitValue, bestSplitFeatureId);
        bestSplitLeft = std::move(leftSplit);
        bestSplitRight = std::move(rightSplit);
        bestSplitUnknown = std::move(unknownSplit);
      }

      // Update constraints (see line 1289 of python code)
      for (const auto &c : constraints) {
        auto cLeftOpt =
            c.propagateLeft(attacker, bestSplitFeatureId, bestSplitValue,
                            dataset.isFeatureNumerical(bestSplitFeatureId));
        if (cLeftOpt.has_value()) {
          constraintsLeft.push_back(cLeftOpt.value());
        }
        auto cRightOpt =
            c.propagateRight(attacker, bestSplitFeatureId, bestSplitValue,
                             dataset.isFeatureNumerical(bestSplitFeatureId));
        if (cRightOpt.has_value()) {
          constraintsRight.push_back(cRightOpt.value());
        }
      }
      // Manage the unknown indexes, where are they going?
      // see loop at line 1299
      indexes_t unknownIndexesToLeft;
      indexes_t unknownIndexesToRight;
      for (const auto &unknownIndex : bestSplitUnknown) {
        const auto instance = dataset.getRecord(unknownIndex);
        const auto attacks = attacker.attack(instance, bestSplitFeatureId,
                                             costs.at(unknownIndex));

        // Assuming the
        cost_t costMinLeft = attacks[0].second;
        // TODO: now I should find the minCost among the attacks, on the left
        // side
        //       there is always an attack with the original instance (that has
        //       the lowest cost and, I suppose, is the first)
        //       is it ok?
        const auto diffLeft = dataset(unknownIndex) - bestPredLeft;
        const auto unknownToLeft = diffLeft < 0.0 ? -diffLeft : diffLeft;
        const auto diffRight = dataset(unknownIndex) - bestPredRight;
        const auto unknownToRight = diffRight < 0.0 ? -diffRight : diffRight;
        if (unknownToLeft > unknownToRight) { // see line 1324 python
          costsLeft[unknownIndex] = costMinLeft;
          // Update the left indexes
          unknownIndexesToLeft.emplace_back(unknownIndex);
          // Checked with Lucchese: it is correct to have bestPredRight as bound
          constraintsLeft.emplace_back(instance, dataset(unknownIndex),
                                       costMinLeft, true, bestPredRight);
          constraintsRight.emplace_back(instance, dataset(unknownIndex),
                                        costMinLeft, false, bestPredRight);
        } else {
          const cost_t costMinRight = [&]() {
            std::vector<cost_t> costsOnRight;
            for (const auto &atk : attacks) {
              if (dataset.isFeatureNumerical(bestSplitFeatureId)) {
                if (atk.first[bestSplitFeatureId] > bestSplitValue) {
                  costsOnRight.push_back(atk.second);
                }
              } else {
                if (atk.first[bestSplitFeatureId] != bestSplitValue) {
                  costsOnRight.push_back(atk.second);
                }
              }
            }
            if (costsOnRight.empty()) {
              throw std::runtime_error(
                  "Cannot determine a valid cost for the right");
            }
            std::sort(costsOnRight.begin(), costsOnRight.end());
            return *costsOnRight.begin();
          }();
          costsRight[unknownIndex] = costMinRight;
          // Update the right indexes
          unknownIndexesToRight.emplace_back(unknownIndex);
          // TODO: check with Lucchese if it is correct to have bestPredLeft as
          //       bound
          constraintsLeft.emplace_back(instance, dataset(unknownIndex),
                                       costMinRight, false, bestPredLeft);
          constraintsRight.emplace_back(instance, dataset(unknownIndex),
                                        costMinRight, true, bestPredLeft);
        }
      }
      // Update the left and right costs (no unknown indexes considered)
      for (const auto &leftIndex : bestSplitLeft) {
        costsLeft[leftIndex] = costs.at(leftIndex);
      }
      for (const auto &rightIndex : bestSplitRight) {
        costsRight[rightIndex] = costs.at(rightIndex);
      }
      // Update the indexes on the left and on the right with the unknown
      // indexes
      for (const auto &i : unknownIndexesToLeft) {
        bestSplitLeft.emplace_back(i);
      }
      for (const auto &i : unknownIndexesToRight) {
        bestSplitRight.emplace_back(i);
      }
    } // end if: not ICML2019 strategy
    //
    return true;
  } else {
    return false;
  }
}

double SplitOptimizer::evaluateSplit(const Dataset &dataset,
                                     const indexes_t &rows,
                                     prediction_t prediction) const {
  return getLoss_(dataset, rows, prediction);
}
