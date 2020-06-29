//
// Created by dg on 14/01/20.
//

#include "Node.h"
#include "utils.h"

Node::Node(std::size_t instancesAtTrain, label_t prediction)
    : instancesAtTrain_(instancesAtTrain), prediction_(prediction), bestSplitFeatureId_(std::nullopt),
      bestSplitFeatureValue_(std::nullopt), left_(nullptr), right_(nullptr) {}

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

void Node::setNumberConstraints(std::size_t n) { constraints_ = n; }

void Node::setBestSplitFeatureId(index_t bestFeatureId) {
  bestSplitFeatureId_ = bestFeatureId;
}

void Node::setBestSplitValue(const feature_t &bestSplitFeatureValue) {
  bestSplitFeatureValue_ = bestSplitFeatureValue;
}

void Node::setBestSplitValueDescription(const std::string &description) {
  bestSplitValueDescription_ = description;
}

label_t Node::predict(const feature_t *record) const {

  if (!left_ && !right_) {
    // leaf
    return getNodePrediction();
  } else if (left_ && right_) {
    // internal node
    if (!(bestSplitFeatureId_.has_value() &&
          bestSplitFeatureValue_.has_value())) {
      throw std::runtime_error("Internal node cannot predict because undefined "
                               "bestFeatureId and bestFeatureValue");
    }
    if (bestSplitValueDescription_.has_value()) {
      // is categorical
      if (record[bestSplitFeatureId_.value()] ==
          bestSplitFeatureValue_.value()) {
        return left_->predict(record);
      } else {
        return right_->predict(record);
      }
    } else {
      // is numerical
      if (record[bestSplitFeatureId_.value()] <=
          bestSplitFeatureValue_.value()) {
        return left_->predict(record);
      } else {
        return right_->predict(record);
      }
    }
  }

  throw std::runtime_error("Node is not a leaf or an internal node");
}

std::string Node::stringify() const {
  if (!left_ && !right_) {
    return utils::format("Prediction = {}, Score: {}, Num. instances {}, Loss: "
                         "{}",
                         static_cast<int>(prediction_), predictionScore_,
                         instancesAtTrain_, lossValue_);
  }
  return utils::format("Feature ID: {}; Threshold = {}, N. instances {}, Loss: "
                       "{}, Gain: {}, N. constraints {}",
                       bestSplitFeatureId_.value(),
                       bestSplitValueDescription_.has_value()
                           ? utils::format("'{}'", bestSplitValueDescription_.value())
                           : std::to_string(bestSplitFeatureValue_.value()),
                       instancesAtTrain_, lossValue_, gainValue_, constraints_);
}
