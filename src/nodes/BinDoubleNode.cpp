//
// Created by dg on 25/11/19.
//

#include "nodes/BinDoubleNode.h"
#include <stdexcept>

BinDoubleNode::BinDoubleNode(index_t featureIndex, double_feature_t v,
                             INode *leftChild, INode *rightChild)
    : featureIndex_(featureIndex), v(v), leftChild(leftChild),
      rightChild(rightChild) {}

BinDoubleNode::~BinDoubleNode() {
  delete leftChild;
  leftChild = nullptr;
  delete rightChild;
  rightChild = nullptr;
}

std::vector<INode *> BinDoubleNode::getChildren() const {
  return {leftChild, rightChild};
}

void BinDoubleNode::setChild(std::size_t index, INode *newNodePtr) {
  switch (index)
  {
  case 0:
    leftChild = newNodePtr;
    break;
  case 1:
    rightChild = newNodePtr;
    break;
  default:
    throw std::runtime_error(
        "Invalid usage of setChild in BinDoubleNode, index is invalid");
  }
}

void BinDoubleNode::setFeatureIndex(std::size_t index) {
  featureIndex_ = index;
}
