//
// Created by dg on 14/01/20.
//

#ifndef TREEANT_NODE_H
#define TREEANT_NODE_H

#include "DecisionTree.h"
#include "types.h"
#include <optional>

class Node {

public:
  Node();
  Node(bool prediction);
  ~Node();

  [[nodiscard]] label_t predict(const record_t &) const;

  friend std::ostream &operator<<(std::ostream &os, const DecisionTree &dt);

  std::optional<bool> prediction_;
  std::optional<double> bestSplitFeatureId_;
  std::optional<double> bestSplitValue_;
  Node *left_;
  Node *right_;

  [[nodiscard]] std::string stringify() const;
};

#endif // TREEANT_NODE_H
