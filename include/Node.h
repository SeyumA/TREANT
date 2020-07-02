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
  explicit Node(std::size_t instancesAtTrain, label_t prediction);
  ~Node();

  void setLeft(Node* left);
  void setRight(Node* right);
  void setNodePrediction(prediction_t prediction, prediction_t threshold = 0.5);
  void setLossValue(double value);
  void setGainValue(double value);
  void setNumberConstraints(std::size_t n);
  void setBestSplitFeatureId(index_t bestFeatureId);
  void setBestSplitValue(const feature_t &bestSplitFeatureValue);
  void setBestSplitValueDescription(const std::string &description);

  [[nodiscard]] prediction_t getNodePrediction() const;
  [[nodiscard]] prediction_t getNodePredictionScore() const;

  [[nodiscard]] label_t predict(const feature_t *record, const bool score) const;

  friend std::ostream &operator<<(std::ostream &os, const DecisionTree &dt);

private:
  std::size_t instancesAtTrain_ = 0;
  prediction_t prediction_;       // it seems to be the predictionScore_ but rounded
  prediction_t predictionScore_;  // floating point
  double lossValue_;
  gain_t gainValue_;
  std::size_t constraints_ = 0;
  std::optional<index_t> bestSplitFeatureId_;
  std::optional<feature_t> bestSplitFeatureValue_;
  std::optional<std::string> bestSplitValueDescription_;
  Node *left_ = nullptr;
  Node *right_ = nullptr;

  [[nodiscard]] std::string stringify() const;
};

#endif // TREEANT_NODE_H
