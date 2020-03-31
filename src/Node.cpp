//
// Created by dg on 14/01/20.
//

#include "Node.h"
#include "utils.h"

Node::Node()
    : prediction_(0.0), bestSplitFeatureId_(std::nullopt),
      bestSplitValue_(std::nullopt), left_(nullptr), right_(nullptr) {}

Node::Node(label_t prediction)
    : prediction_(prediction), bestSplitFeatureId_(std::nullopt),
      bestSplitValue_(std::nullopt), left_(nullptr), right_(nullptr) {}

Node::Node(index_t bestSplitFeatureId, feature_t bestSplitValue)
    : prediction_(0.0), bestSplitFeatureId_(bestSplitFeatureId),
      bestSplitValue_(bestSplitValue), left_(nullptr), right_(nullptr) {}

Node::~Node() {
  delete left_;
  delete right_;
  left_ = nullptr;
  right_ = nullptr;
}

void Node::setLeft(Node *left) { left_ = left; }

void Node::setRight(Node *right) { right_ = right; }

void Node::setNodePrediction(prediction_t predictionScore,
                             prediction_t threshold) {
  predictionScore_ = predictionScore;
  if (predictionScore < threshold) {
    prediction_ = 0.0;
  } else {
    prediction_ = 1.0;
  }
}

prediction_t Node::getNodePrediction() const { return prediction_; }

prediction_t Node::getNodePredictionScore() const { return predictionScore_; }

void Node::setLossValue(double value) { lossValue_ = value; }

void Node::setGainValue(double value) { gainValue_ = value; }

void Node::setBestSplitFeatureId(index_t bestFeatureId) {
  bestSplitFeatureId_ = bestFeatureId;
}

void Node::setBestSplitValue(feature_t bestSplitValue) {
  bestSplitValue_ = bestSplitValue;
}

label_t Node::predict(const record_t &) const { return false; }

std::string Node::stringify() const {
  if (!left_ && !right_) {
    return utils::format("Prediction = {}, Score: {}", prediction_,
                         predictionScore_);
  }
  return utils::format("Feature ID: {}; Threshold = {}", bestSplitFeatureId_.value(),
                       bestSplitValue_.value());
}
