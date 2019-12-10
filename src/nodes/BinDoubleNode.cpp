//
// Created by dg on 25/11/19.
//

#include "nodes/BinDoubleNode.h"
#include <stdexcept>

BinDoubleNode::BinDoubleNode(index_t featureIndex, double_feature_t v)
    : INode(featureIndex, {nullptr, nullptr}), v_(v) {}

label_t BinDoubleNode::predict(const record_t &record) const {
  const auto doubleFeaturePtr =
      std::get_if<double_feature_t>(&record[featureIndex_]);
  if (!doubleFeaturePtr) {
    throw std::runtime_error(
        "BinDoubleNode::predict() called on a non-double_feature_t");
  } else if (*doubleFeaturePtr > v_) {
    return children_[1]->predict(record);
  } else {
    return children_[0]->predict(record);
  }
}

[[nodiscard]] partitions_t
BinDoubleNode::split(const partition_t &validIndexes,
                     const feature_vector_t &featureVector) const {
  const auto doubleVectorPtr = std::get_if<double_vector_t>(&featureVector);
  if (!doubleVectorPtr) {
    throw std::runtime_error(
        "BinDoubleNode::split() called on a non-double_vector_t");
  }
  // Create the 2 partitions
  std::vector<std::vector<index_t>> p(2);
  for (const auto &i : validIndexes) {
    if ((*doubleVectorPtr)[i] > v_) {
      p[1].push_back(i);
    } else {
      p[0].push_back(i);
    }
  }
  //
  return p;
}

std::string BinDoubleNode::stringify() const {
  std::ostringstream oss;
  oss << "BinDoubleNode(feature index = " << featureIndex_ << ")";
  return oss.str();
}
