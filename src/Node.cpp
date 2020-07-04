//
// Created by dg on 14/01/20.
//

#include <cstdlib>
#include <iomanip>
#include <sstream>
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
} // namespace keys

Node::Node(std::size_t instancesAtTrain, label_t prediction)
    : instancesAtTrain_(instancesAtTrain), prediction_(prediction),
      bestSplitFeatureId_(std::nullopt), bestSplitFeatureValue_(std::nullopt),
      left_(nullptr), right_(nullptr) {}

Node::Node(const std::string &s) {
  std::unordered_map<std::string, std::string> map;
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

  // using strtod because it preserves inf and NaN
  if (map.find(keys::FeatureID) == map.end()) {
    // leaf
    instancesAtTrain_ = std::stoul(map[keys::NumInstances]);
    lossValue_ = strtod(map[keys::Loss].c_str(), NULL);
    prediction_ = strtod(map[keys::Prediction].c_str(), NULL);
    predictionScore_ = strtod(map[keys::Score].c_str(), NULL);
    bestSplitFeatureId_ = std::nullopt;
    bestSplitFeatureValue_ = std::nullopt;
    bestSplitValueDescription_ = std::nullopt;
    left_ = nullptr;
    right_ = nullptr;
  } else {
    // internal node
    instancesAtTrain_ = std::stoul(map[keys::NumInstances]);
    lossValue_ = strtod(map[keys::Loss].c_str(), NULL);
    gainValue_ = strtod(map[keys::Gain].c_str(), NULL);
    bestSplitFeatureId_ = std::stoul(map[keys::FeatureID]);
    bestSplitFeatureValue_ = strtod(map[keys::Threshold].c_str(), NULL);
    if (map.find(keys::Description) != map.end()) {
      bestSplitValueDescription_ = map[keys::Description];
    } else {
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

std::size_t Node::getSubtreeHeight() const {
  if (!left_ && !right_) {
    return 1;
  } else if (left_ && right_) {
    const auto leftHeight = left_->getSubtreeHeight();
    const auto rightHeight = right_->getSubtreeHeight();
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
  }
  throw std::runtime_error("Node must be a leaf or an internal node");
}

std::size_t Node::getSubtreeNumberNodes() const {
  if (!left_ && !right_) {
    return 1;
  } else if (left_ && right_) {
    const auto leftCount = left_->getSubtreeNumberNodes();
    const auto rightCount = right_->getSubtreeNumberNodes();
    return 1 + leftCount + rightCount;
  }
  throw std::runtime_error("Node must be a leaf or an internal node");
}

void Node::getSubtreeStruct(std::ostream &os) const {
  // rootId is the assigned Id to the root (the last node written to os stream)
  const auto rootId = getSubtreeStruct(os, 0);
  if (rootId + 1 != getSubtreeNumberNodes()) {
    throw std::runtime_error("");
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
    // leaf: get the node predicion score (see orginal python code where they
    // choose getNodePrediction()[1] because getNodePrediction returns a tuple
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
  // using std::numeric_limits<double>::max_digits10 in order to have better
  // precision on floating point numbers
  if (!left_ && !right_) {
    std::stringstream ss;
    ss << keys::Prediction << ':'
       << std::setprecision(std::numeric_limits<double>::max_digits10)
       << prediction_ << ',';
    ss << keys::Score << ':'
       << std::setprecision(std::numeric_limits<double>::max_digits10)
       << predictionScore_ << ',';
    ss << keys::NumInstances << ':' << instancesAtTrain_ << ',';
    ss << keys::Loss << ':' << lossValue_;
    return ss.str();
  } else if (left_ && right_) {

    std::stringstream ss;
    ss << keys::FeatureID << ':' << bestSplitFeatureId_.value() << ',';
    ss << keys::Threshold << ':'
       << std::setprecision(std::numeric_limits<double>::max_digits10)
       << bestSplitFeatureValue_.value() << ',';
    if (bestSplitValueDescription_.has_value()) {
      ss << keys::Description << ':' << bestSplitValueDescription_.value()
         << ',';
    }
    ss << keys::NumInstances << ':' << instancesAtTrain_ << ',';
    ss << keys::Loss << ':'
       << std::setprecision(std::numeric_limits<double>::max_digits10)
       << lossValue_ << ',';
    ss << keys::Gain << ':'
       << std::setprecision(std::numeric_limits<double>::max_digits10)
       << gainValue_ << ',';
    ss << keys::NumConstraints << ':' << constraints_;
    return ss.str();
  } else if (!left_) {
    return "WARNING: this node has left node equal to null (" +
           std::to_string(instancesAtTrain_) + " instances fall into it)";
  }
  return "WARNING: this node has right node equal to null (" +
         std::to_string(instancesAtTrain_) + " instances fall into it)";
}

// std::ostream &os can be a file stream, freeId is the free ID propagated
// downstream that will be used by the next node written to os stream.
// The returning value is the last id written to the os stream.
std::size_t Node::getSubtreeStruct(std::ostream &os,
                                   const std::size_t freeId) const {
  if (!left_ && !right_) {
    os << stringify() << std::endl;
    return freeId;
  } else if (left_ && right_) {
    const auto leftId = left_->getSubtreeStruct(os, freeId);
    // leftId is the id used by the left child, the next free ID is leftId + 1
    const auto rightId = right_->getSubtreeStruct(os, leftId + 1);
    // Write the current node to the stream
    os << '[' << leftId << ',' << rightId << ']' << stringify() << std::endl;
    // Its ID is rightId + 1
    return rightId + 1;
  }
  throw std::runtime_error("Node must be a leaf or an internal node");
}
