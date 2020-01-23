//
// Created by dg on 01/11/19.
//

#ifndef TREEANT_DECISIONTREE_H
#define TREEANT_DECISIONTREE_H

#include <iosfwd>
#include <vector>

#include "Dataset.h"
#include "SplitOptimizer.h"

class Node;

class DecisionTree final {

public:
  enum class VisitorConstructorTypes { GINI = 0x0 };

  // Constructors
  DecisionTree() = delete;

  DecisionTree(DecisionTree &) = delete;

  DecisionTree(DecisionTree &&) = delete;

  explicit DecisionTree(const Dataset &dataset, const std::size_t &maxDepth,
                        VisitorConstructorTypes visitorType);

  // Destructor
  ~DecisionTree();

  // Functions
  [[nodiscard]] label_t predict(const record_t &) const;

  [[nodiscard]] std::size_t getHeight() const;

  void fit(const Dataset &dataset, SplitOptimizer::Impurity impurityType);

  friend std::ostream &operator<<(std::ostream &os, const DecisionTree &dt);

private:
  std::size_t maxDepth_;
  std::size_t height_ = 0;
  Node *root_ = nullptr;
  bool isTrained_;

  Node *fitRecursively(const Dataset &dataset,
                       const std::vector<std::size_t> &rows,
                       const std::vector<bool> &blackList,
                       std::size_t currHeight /*attacker, cost, ...*/,
                       const Attacker &attacker,
                       SplitOptimizer::Impurity impurityType);
};

std::ostream &operator<<(std::ostream &os, const DecisionTree &dt);

#endif // TREEANT_DECISIONTREE_H
