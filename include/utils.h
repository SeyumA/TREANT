//
// Created by dg on 31/10/19.
//

#ifndef TREEANT_UTILS_H
#define TREEANT_UTILS_H

#include "types.h"
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>

class Node;
class Dataset;
class Constraint;
class Attacker;

namespace utils {

std::map<std::string, std::string> get_options_map(const std::string &args);

std::string join(const std::vector<std::string> &list, const char delimiter);

template <typename S1, typename S2> std::string concatenate(S1 s1, S2 s2) {
  std::stringstream ss;
  ss << std::fixed << s1 << s2;
  return ss.str();
}

template <typename First, typename... Types>
std::string format(std::string firstArg, First head, Types... args) {
  // NOTE: the check on {} can be done at compile time
  // (see
  // https://akrzemi1.wordpress.com/2011/05/11/parsing-strings-at-compile-time-part-i/)
  const auto pos = firstArg.find("{}");
  if constexpr (sizeof...(args) != 0) {
    if (pos == std::string::npos) {
      throw std::runtime_error("You are lacking '{}' in format()");
    }
    auto dummy = concatenate(firstArg.substr(0, pos), head);
    return dummy + format(firstArg.substr(pos + 2), args...);
  } else {
    const auto trailPart = firstArg.substr(pos + 2);
    if (trailPart.find("{}") != std::string::npos) {
      throw std::runtime_error("Too many '{}' in format()");
    }
    return concatenate(firstArg.substr(0, pos), head) + trailPart;
  }
}
//
//// Parallel split optimizer
// static double sseCostFunction(const std::vector<double> &x,
//                              std::vector<double> &grad, void *data);
//
// static double constraintFunction(const std::vector<double> &x,
//                                 std::vector<double> &grad, void *data);

struct OptimizeOutput {
  gain_t bestGain;
  index_t bestSplitFeatureId;
  split_value_t bestSplitValue;
  split_value_t bestNextSplitValue;
  prediction_t bestPredLeft;
  prediction_t bestPredRight;
  double bestSSEuma;
};

OptimizeOutput optimizeOnSubset(
    const Dataset &dataset, const std::unordered_map<index_t, cost_t> &costs,
    const Attacker &attacker, const indexes_t &validFeaturesSubset,
    const indexes_t &validInstances, const std::vector<Constraint> &constraints,
    const double &currentScore, const double &currentPredictionScore,
    const bool &useICML2019);

// Returns left, unknown, right
std::tuple<indexes_t, indexes_t, indexes_t>
simulateSplit(const Dataset &dataset, const indexes_t &validInstances,
              const Attacker &attacker,
              const std::unordered_map<index_t, cost_t> &costs,
              const feature_t &splittingValue, const index_t &splittingFeature);

std::tuple<indexes_t, indexes_t, indexes_t, bool> simulateSplitICML2019(
    const Dataset &dataset, const indexes_t &validInstances,
    const Attacker &attacker, const std::unordered_map<index_t, cost_t> &costs,
    const feature_t &splittingValue, const index_t &splittingFeature,
    label_t &yHatLeft, label_t &yHatRight, gain_t &sse);


std::vector<indexes_t> buildBatches(const unsigned &numThreads,
                                    const indexes_t &validFeatures);

} // namespace utils

#endif // TREEANT_UTILS_H
