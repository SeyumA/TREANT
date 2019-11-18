//
// Created by dg on 06/11/19.
//

#include "nodes/BooleanNode.h"
#include <stdexcept>

BooleanNode::BooleanNode(std::size_t featureIndex)
    : featureIndex_(featureIndex) {}

BooleanNode ::~BooleanNode() {
  delete leftChild;
  delete rightChild;
}

//int BooleanNode::predict(const record_t &r) const {
//  if (std::get<bool>(r[featureIndex_])) {
//    return leftChild->predict(r);
//  } else {
//    return rightChild->predict(r);
//  }
//}

std::vector<INode *> BooleanNode::getChildren() const {
  return {leftChild, rightChild};
}

void BooleanNode::setChild(std::size_t index, INode *newNodePtr) {
  if (index == 0) {
    leftChild = newNodePtr;
  } else if (index == 1) {
    rightChild = newNodePtr;
  } else {
    throw std::runtime_error(
        "Invalid usage of setChild in BinIntNode, index is invalid");
  }
}

void BooleanNode::setFeatureIndex(std::size_t index) { featureIndex_ = index; }
