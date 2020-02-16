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
  explicit Node(bool prediction);
  explicit Node(index_t bestSplitFeatureId, feature_t bestSplitValue);
  ~Node();

  void setLeft(Node* left);
  void setRight(Node* right);

  [[nodiscard]] label_t predict(const record_t &) const;

  friend std::ostream &operator<<(std::ostream &os, const DecisionTree &dt);

private:
  std::optional<bool> prediction_;
  std::optional<std::size_t> bestSplitFeatureId_;
  std::optional<double> bestSplitValue_;
  Node *left_;
  Node *right_;

  [[nodiscard]] std::string stringify() const;
};

#endif // TREEANT_NODE_H
