//
// Created by dg on 05/11/19.
//

#include "nodes/BinIntNode.h"
#include <stdexcept>

BinIntNode::BinIntNode(index_t featureIndex, int_feature_t v)
    : INode(featureIndex, {nullptr, nullptr}), v_(v) {}

partitions_t BinIntNode::split(const partition_t &validIndexes,
                               const feature_vector_t &featureVector) const {
  const auto intVectorPtr = std::get_if<int_vector_t>(&featureVector);
  if (!intVectorPtr) {
    throw std::runtime_error(
        "BinIntNode::split() called on a non-int_vector_t");
  }
  // Create the 2 partitions
  partitions_t p(2);
  for (const auto &i : validIndexes) {
    // Here there is also the possibility of the "unknown instances of D"
    // (*intVectorPtr)[i] == v_ (see pag. 4 of the article).
    if ((*intVectorPtr)[i] > v_) {
      p[1].push_back(i);
    } else {
      p[0].push_back(i);
    }
  }
  //
  return p;
}

label_t BinIntNode::predict(const record_t &record) const {
  const auto intFeaturePtr = std::get_if<int_feature_t>(&record[featureIndex_]);
  if (!intFeaturePtr) {
    throw std::runtime_error(
        "BinIntNode::predict() called on a non-int_feature_t");
  } else if (*intFeaturePtr > v_) {
    return children_[1]->predict(record);
  } else {
    return children_[0]->predict(record);
  }
}

std::string BinIntNode::stringify() const {
  std::ostringstream oss;
  oss << "BinIntNode(feature index = " << featureIndex_ << ")";
  return oss.str();
}
