//
// Created by dg on 14/01/20.
//

#include <unordered_map>

#include "Node.h"
#include "utils.h"

namespace keys {
    // leaf keys
    static const std::string Prediction = "Prediction";
    static const std::string Score = "Score";
    static const std::string NumInstances = "Num_instances";
    static const std::string Loss = "Loss";
    // internal node keys (also NumInstances and Loss)
    static const std::string FeatureID = "Feature_ID";
    static const std::string Threshold = "Threshold";
    static const std::string Description = "Description";
    static const std::string Gain = "Gain";
    static const std::string NumConstraints = "Num_constraints";
}

Node::Node(std::size_t instancesAtTrain, label_t prediction)
    : instancesAtTrain_(instancesAtTrain), prediction_(prediction), bestSplitFeatureId_(std::nullopt),
      bestSplitFeatureValue_(std::nullopt), left_(nullptr), right_(nullptr) {}

Node::Node(const std::string& s) {
    std::unordered_map<std::string,std::string> map;
    std::istringstream iss(s);
    std::string token;
    char delimiter = ',';
    char splitter = ':';
    while (std::getline(iss, token, delimiter)) {
        const auto found = token.find(splitter);
        if (found == std::string::npos) {
            throw std::runtime_error("Cannot split: " + token);
        }
        const auto key = token.substr(0, found);
        const auto value = token.substr(found + 1);
        map[key] = value;
    }

    if (map.find(keys::Threshold) == map.end()) {
        // leaf
        instancesAtTrain_ = std::stoul(map[keys::NumInstances]);
        lossValue_ = std::stod(map[keys::Loss]);
        bestSplitFeatureId_ = std::nullopt;
        bestSplitFeatureValue_ = std::nullopt;
        left_ = nullptr;
        right_ = nullptr;
    } else {
        // internal node
        instancesAtTrain_ = std::stoul(map[keys::NumInstances]);
        lossValue_ = std::stod(map[keys::Loss]);
        gainValue_ = std::stod(map[keys::Gain]);
        bestSplitFeatureId_ = std::stoul(map[keys::FeatureID]);
        bestSplitFeatureValue_ = std::stod(map[keys::Threshold]);
        if (map.find(keys::Description) != map.end()) {
            bestSplitValueDescription_ = map[keys::Description];
        }else {
            bestSplitValueDescription_ = std::nullopt;
        }
        constraints_ = std::stoul(map[keys::NumConstraints]);
        left_ = nullptr;
        right_ = nullptr;
    }
}

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

label_t Node::predict(const feature_t *record, const bool score) const {

  if (!left_ && !right_) {
    // leaf: get the node predicion score (see orginal python code where they choose
    // getNodePrediction()[1] because getNodePrediction returns a tuple
    return score ? getNodePredictionScore() : getNodePrediction();
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
        return left_->predict(record, score);
      } else {
        return right_->predict(record, score);
      }
    } else {
      // is numerical
      if (record[bestSplitFeatureId_.value()] <=
          bestSplitFeatureValue_.value()) {
        return left_->predict(record, score);
      } else {
        return right_->predict(record, score);
      }
    }
  }

  throw std::runtime_error("Node is not a leaf or an internal node");
}

std::string Node::stringify() const {
  // TODO: enhance this one in order to have a better approximation
  if (!left_ && !right_) {
    return utils::format("{}:{},{}:{},{}:{},{}:{}",
                         keys::Prediction, prediction_, keys::Score, predictionScore_, keys::NumInstances,
                         instancesAtTrain_, keys::Loss, lossValue_);
  }
  return utils::format("{}:{},{}:{}{},{}:{},{}:{},"
                       "{}:{},{}:{}",
                       keys::FeatureID, bestSplitFeatureId_.value(),
                       keys::Threshold, bestSplitFeatureValue_.value(),
                       bestSplitValueDescription_.has_value()
                           ? utils::format(",{}:{}", keys::Description, bestSplitValueDescription_.value())
                           : "",
                       keys::NumInstances, instancesAtTrain_, keys::Loss, lossValue_, keys::Gain, gainValue_,
                       keys::NumConstraints, constraints_);
}
