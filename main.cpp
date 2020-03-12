#include <iostream>
#include <queue>

#include "Dataset.h"
#include "DecisionTree.h"
#include "SplitOptimizer.h"

double budget = 50.0;
double EPS = 1e-7;

std::pair<double, double> getCostAndPostOfAttack(int featureId) {
  if (featureId == 0) {
    return {10.0, -1.0};
  } else if (featureId == 1) {
    return {15.0, -1.0};
  } else if (featureId == 2) {
    return {25.0, -1.0};
  } else {
    throw std::runtime_error("Invalid feature Id");
  }
}

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

  /*
   *
   *                                         0000(1)
   *            1000      ->           0100          0010     0001
   *    1100    1010  1001           0110 0101       0011
   * 1110 1101  1011                 0111
   * 1111
   *
   *
   *
   * */
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
