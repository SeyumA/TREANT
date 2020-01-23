//
// Created by dg on 14/01/20.
//

#include "SplitOptimizer.h"
#include <stdexcept>
SplitOptimizer::SplitOptimizer(SplitOptimizer::Impurity impurityType)
    : impurityType_(impurityType) {}

double SplitOptimizer::getLoss(const std::vector<bool> &yTrue, double yPred) {
  // Consider all the possible cases of impurity type
  switch (impurityType_) {
  case Impurity::GINI:
    throw std::runtime_error("Implement GINI");
  case Impurity::SSE:
    if (yTrue.empty()) {
      return 0.0;
    }

    return [&yTrue, &yPred]() -> double {
      double sum = 0.0;
      for (const auto &y : yTrue) {
        double yAsDouble = y ? 1.0 : 0.0;
        double diff = yAsDouble - yPred;
        sum += diff * diff;
      }
      return sum;
    }();

  case Impurity::MSE:
    throw std::runtime_error("Implement MSE");
  case Impurity::ENTROPY:
    throw std::runtime_error("Implement ENTROPY");

  default:
    throw std::runtime_error("Not valid impurity type");
  }
}

std::tuple<double, double, double> SplitOptimizer::optimizeSSEunderMaxAttack(
    const std::vector<bool> &y, const std::vector<std::size_t> &leftIndexes,
    const std::vector<std::size_t> &rightIndexes,
    const std::vector<std::size_t> &unknownIndexes) {

  //TODO implement this also with static

  return std::tuple<double, double, double>();
}

std::tuple<std::size_t, double> SplitOptimizer::optimize_gain(
    const Dataset &dataset, const std::vector<std::size_t> &rows,
    const std::vector<bool> &blackList, const Attacker &attacker,
    std::vector<std::size_t> &costs) {

  // TODO: implement this

  return {0, 1.5};
}
