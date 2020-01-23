//
// Created by dg on 14/01/20.
//

#include "Node.h"
#include "utils.h"

Node::Node()
    : bestSplitFeatureId_(std::nullopt), bestSplitValue_(std::nullopt),
      left_(nullptr), right_(nullptr) {}

Node::Node(bool prediction)
    : prediction_(prediction), bestSplitValue_(std::nullopt), left_(nullptr),
      right_(nullptr) {}

Node::~Node() {
  delete left_;
  delete right_;
  left_ = nullptr;
  right_ = nullptr;
}

label_t Node::predict(const record_t &) const { return false; }

std::string Node::stringify() const {
  return utils::format(
      "th = {}", bestSplitValue_.has_value() ? bestSplitValue_.value() : -1);
}
