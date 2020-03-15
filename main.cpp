#include <iostream>
#include <queue>

#include "Dataset.h"
#include "DecisionTree.h"
#include "SplitOptimizer.h"

double budget = 50;
 double EPS = 1e-7;

std::pair<int, int> getCostAndPostOfAttack(int featureId) {
  if (featureId == 0) {
    return {10, -1};
  } else if (featureId == 1) {
    return {15, -1};
  } else if (featureId == 2) {
    return {25, -1};
  } else {
    throw std::runtime_error("Invalid feature Id");
  }
}

struct SimpleAttackRule {
  SimpleAttackRule(int indexToAttack, int cost, std::vector<int> pre, int post,
                   bool isNumerical)
      : indexToAttack(indexToAttack), cost(cost), pre(std::move(pre)),
        post(post), isNumerical(isNumerical) {
    if (pre.empty() || !(isNumerical && pre.size() == 2 && pre[0] <= pre[1])) {
      throw std::runtime_error(
          "Invalid parameters in SimpleAttackRule constructor");
    }
  }

  bool apply(const std::vector<int> &instance, const int &currentCost,
             std::vector<int> &newInstance, int &newCost) {
    if (indexToAttack < 0 || (unsigned)indexToAttack >= instance.size()) {
      return false;
    }
    if (isNumerical && instance[indexToAttack] >= pre[0] &&
        instance[indexToAttack] <= pre[1]) {
      newInstance = std::vector<int>(instance);
      newInstance[indexToAttack] += post;
      newCost = currentCost + cost;
      return true;
    } else if (!isNumerical) {
      for (const auto &i : pre) {
        if (i == instance[indexToAttack]) {
          newInstance = std::vector<int>(instance);
          newInstance[indexToAttack] = post;
          newCost = currentCost + cost;
          return true;
        }
      };
    }
    return false;
  }

  int indexToAttack;
  int cost;
  std::vector<int> pre;
  int post;
  bool isNumerical;
};

void generateAttacksRic(
    const std::vector<int> &featureIdsToAttack,
    std::vector<std::pair<std::vector<double>, double>> &accumulator) {

  // Get the last instance inserted, it is our basis
  const auto &[instance, currentCost] = accumulator.back();

  // Generate the 2^n possible attacked instances, where n = validIds.size()
  std::queue<
      std::tuple<std::vector<double>, std::vector<bool>, double, unsigned>>
      q;
  q.push(std::make_tuple(instance,
                         std::vector<bool>(featureIdsToAttack.size(), false),
                         currentCost, 0));
  while (!q.empty()) {
    const auto [attackedInstance, attackedActiveVector, attackedCost,
                firstIdToAttack] = q.front();
    q.pop();
    for (unsigned i = firstIdToAttack; i < attackedActiveVector.size(); i++) {
      const auto featureIdToAttack = featureIdsToAttack[i];
      const auto [cost, post] = getCostAndPostOfAttack(featureIdToAttack);
      const auto expectedCost = attackedCost + cost;
      if (expectedCost <= EPS + budget) {
        // Create the new instance
        std::vector<double> newInstance(attackedInstance);
        newInstance[featureIdToAttack] += post; // if numerical ...
        // Update the queue
        std::vector<bool> newAttackedActiveVector(attackedActiveVector);
        newAttackedActiveVector[i] = true;
        q.push(std::make_tuple(newInstance, newAttackedActiveVector,
                               expectedCost, i + 1));
        accumulator.emplace_back(newInstance, expectedCost);
        // Print the new feature added
        std::cout << "{ ";
        for (const auto &d : newInstance) {
          std::cout << d << " ";
        }
        std::cout << "} cost: " << expectedCost << std::endl;
        // Prepare the new feature ids to attack
        std::vector<int> newFeatureIdsToAttack;
        for (unsigned ii = 0; ii < newAttackedActiveVector.size(); ii++) {
          if (newAttackedActiveVector[ii]) {
            newFeatureIdsToAttack.emplace_back(featureIdsToAttack[ii]);
          }
        }
        // recursive call
        generateAttacksRic(newFeatureIdsToAttack, accumulator);
      }
    } // end of for loop for children generation
  }
}

std::vector<std::pair<std::vector<double>, double>> generateAttacks() {
  std::vector<double> instance = {budget, budget, budget};
  std::vector<int> validIndexes = {0, 1, 2};
  std::vector<std::pair<std::vector<double>, double>> ret;
  ret.emplace_back(instance, 0.0);
  generateAttacksRic(validIndexes, ret);

  int totalPosCases = 0;
  int maxIter = 6;
  const auto costI = getCostAndPostOfAttack(0).first;
  const auto costJ = getCostAndPostOfAttack(1).first;
  const auto costK = getCostAndPostOfAttack(2).first;
  for (int i = 0; i < maxIter; i++) {
    for (int j = 0; j < maxIter; j++) {
      for (int k = 0; k < maxIter; k++) {
        double cost = 0.0;
        for (int ii = 0; ii < i; ii++) {
          cost += costI;
        }
        for (int jj = 0; jj < j; jj++) {
          cost += costJ;
        }
        for (int kk = 0; kk < k; kk++) {
          cost += costK;
        }
        if (cost < EPS + budget) {
          totalPosCases++;
        }
      }
    }
  }

  std::cout << "Total cases should be " << totalPosCases << std::endl;
  std::cout << "Total returned cases are " << ret.size() << std::endl;

  return ret;
}

int main() {

  // TEST nlopt
  //  SplitOptimizer::optimizeNLOPTtest();
  const auto attacks = generateAttacks();
  std::cout << "\nAttacks:\n";
  for (const auto &[instance, cost] : attacks) {
    std::cout << "{ ";
    for (const auto &d : instance) {
      std::cout << d << " ";
    }
    std::cout << "} cost: " << cost << std::endl;
  }
  // END test nlopt

  std::string dataset_file_path =
      "/home/dg/source/repos/uni/treeant/data/test_training_set.txt";
  Dataset dataset(dataset_file_path);
  std::cout << "The dataset is:" << std::endl << dataset << std::endl;
  std::size_t maxDepth = 4;
  std::size_t budget = 40;
  DecisionTree dt(maxDepth);
  dt.fit(dataset, budget, Impurity::SSE);
  std::cout << "The decision tree is:" << std::endl << dt << std::endl;
  // Build a test record and try to predict it.
  // The arguments order must match the column feature order in the dataset.
  record_t record{3.2, 9.1, 3.0};

  std::cout << "Is the decision tree trained: " << dt.isTrained() << std::endl;
  return 0;
}
