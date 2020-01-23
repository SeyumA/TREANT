//
// Created by dg on 14/01/20.
//

#ifndef TREEANT_SPLITOPTIMIZER_H
#define TREEANT_SPLITOPTIMIZER_H

#include "Attacker.h"
#include "Dataset.h"
#include <tuple>
#include <vector>

class SplitOptimizer {

public:
  enum class Impurity { GINI, SSE, MSE, ENTROPY };

  explicit SplitOptimizer(Impurity impurityType);

  std::tuple<double, double, double>
  optimizeSSEunderMaxAttack(const std::vector<bool> &y,
                            const std::vector<std::size_t> &leftIndexes,
                            const std::vector<std::size_t> &rightIndexes,
                            const std::vector<std::size_t> &unknownIndexes
                            /*TODO: lossFunction, constraints*/);

  // It returns the best splitting feature and the best splitting value
  std::tuple<std::size_t, double>
  optimize_gain(const Dataset &dataset, const std::vector<std::size_t> &rows,
                const std::vector<bool> &blackList, const Attacker &attacker,
                std::vector<std::size_t> &costs);

private:
  Impurity impurityType_;

  double getLoss(const std::vector<bool> &yTrue, double yPred);
};

#endif // TREEANT_SPLITOPTIMIZER_H
