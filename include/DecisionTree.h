//
// Created by dg on 01/11/19.
//

#ifndef TREEANT_DECISIONTREE_H
#define TREEANT_DECISIONTREE_H

#include "Constraint.h"
#include "types.h"

#include <iosfwd>
#include <unordered_map>
#include <vector>

class Node;
class Dataset;
class Attacker;

class DecisionTree final {

public:
  // Constructor
  explicit DecisionTree(std::size_t maxDepth, bool isAffine = false);

  // Destructor
  ~DecisionTree();

  // Functions
  [[nodiscard]] label_t predict(const record_t &) const;

  [[nodiscard]] std::size_t getHeight() const;

  void fit(const Dataset &dataset, const std::string &attackerFile,
           cost_t budget, const unsigned &threads,
           const Impurity impurityType = Impurity::SSE);

  [[nodiscard]] bool isTrained() const;

  friend std::ostream &operator<<(std::ostream &os, const DecisionTree &dt);

private:
  std::size_t maxDepth_ = 0;
  bool isAffine_ = false;
  std::size_t minPerNode_ = 20;
  std::size_t height_ = 0;
  Node *root_ = nullptr;

  Node *fitRecursively(const Dataset &dataset, const indexes_t &rows,
                       const indexes_t &validFeatures, std::size_t currHeight,
                       const Attacker &attacker,
                       const std::unordered_map<index_t, cost_t> &costs,
                       const prediction_t &currentPrediction,
                       Impurity impurityType,
                       const std::vector<Constraint> &constraints,
                       const unsigned &threads);
};

std::ostream &operator<<(std::ostream &os, const DecisionTree &dt);

#endif // TREEANT_DECISIONTREE_H
