//
// Created by dg on 14/01/20.
//

#ifndef TREEANT_NODE_H
#define TREEANT_NODE_H

#include <optional>

#include "types.h"
#include "DecisionTree.h"

class Node {

public:
  Node();
  explicit Node(label_t prediction);
  explicit Node(index_t bestSplitFeatureId, feature_t bestSplitValue);
  ~Node();

  void setLeft(Node* left);
  void setRight(Node* right);
  void setNodePrediction(prediction_t prediction, prediction_t threshold = 0.5);
  void setLossValue(double value);
  void setGainValue(double value);
  [[nodiscard]] prediction_t getNodePrediction() const;
  [[nodiscard]] prediction_t getNodePredictionScore() const;

  [[nodiscard]] label_t predict(const record_t &) const;

  friend std::ostream &operator<<(std::ostream &os, const DecisionTree &dt);

private:
  prediction_t prediction_;       // it seems to be the predictionScore_ but rounded
  prediction_t predictionScore_;  // floating point
  double lossValue_;
  gain_t gainValue_;
  std::optional<index_t> bestSplitFeatureId_;
  std::optional<feature_t> bestSplitValue_;
  Node *left_ = nullptr;
  Node *right_ = nullptr;

  [[nodiscard]] std::string stringify() const;
};

#endif // TREEANT_NODE_H
