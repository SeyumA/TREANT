//
// Created by dg on 06/11/19.
//

#include "nodes/BooleanNode.h"
#include <stdexcept>

BooleanNode::BooleanNode(index_t featureIndex)
    : INode(featureIndex, {nullptr, nullptr}) {}

partitions_t BooleanNode::split(const partition_t &validIndexes,
                                const feature_vector_t &featureVector) const {
  const auto boolVectorPtr = std::get_if<bool_vector_t>(&featureVector);
  if (!boolVectorPtr) {
    throw std::runtime_error(
        "BooleanNode::split() called on a non-bool_vector_t");
  }
  partitions_t p(2);
  for (const auto &i : validIndexes) {
    if (!(*boolVectorPtr)[i]) {
      p[0].push_back(i);
    } else {
      p[1].push_back(i);
    }
  }
  //
  return p;
}

label_t BooleanNode::predict(const record_t &record) const {
  const auto boolFeaturePtr =
      std::get_if<bool_feature_t>(&record[featureIndex_]);
  if (!boolFeaturePtr) {
    throw std::runtime_error(
        "BooleanNode::predict() called on a non-bool_feature_t");
  } else if (*boolFeaturePtr) {
    return children_[0]->predict(record);
  } else {
    return children_[1]->predict(record);
  }
}

std::string BooleanNode::print() const {
  std::ostringstream oss;
  oss << "BooleanNode(" << featureIndex_ << ")";
  return oss.str();
}
