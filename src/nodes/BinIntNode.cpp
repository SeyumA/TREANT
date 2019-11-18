//
// Created by dg on 05/11/19.
//

#include "nodes/BinIntNode.h"
#include <stdexcept>

BinIntNode::BinIntNode(int featureIndex, int v, INode *leftChild,
                       INode *rightChild)
    : featureIndex_(featureIndex), v(v), leftChild(leftChild),
      rightChild(rightChild) {}

BinIntNode::~BinIntNode() {
  delete leftChild;
  leftChild = nullptr;
  delete rightChild;
  rightChild = nullptr;
}

//int BinIntNode::predict(const record_t &r) const {
//  int record_feature = std::get<int>(r[featureIndex_]);
//  return record_feature <= v ? leftChild->predict(r) : rightChild->predict(r);
//}

std::vector<INode *> BinIntNode::getChildren() const {
  return {leftChild, rightChild};
}

void BinIntNode::setChild(std::size_t index, INode *newNodePtr) {
  if (index == 0) {
    leftChild = newNodePtr;
  } else if (index == 1) {
    rightChild = newNodePtr;
  } else {
    throw std::runtime_error(
        "Invalid usage of setChild in BinIntNode, index is invalid");
  }
}

void BinIntNode::setFeatureIndex(std::size_t index) {
  featureIndex_ = index;
}