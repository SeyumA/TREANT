//
// Created by dg on 05/11/19.
//

#include "nodes/BinIntNode.h"

BinIntNode::BinIntNode(int featureIndex, int v, INode *leftChild,
                       INode *rightChild)
    : featureIndex(featureIndex), v(v), leftChild(leftChild),
      rightChild(rightChild) {}

BinIntNode::~BinIntNode() {
  delete leftChild;
  leftChild = nullptr;
  delete rightChild;
  rightChild = nullptr;
}


int BinIntNode::predict(record_t r) const {
  int record_feature = std::get<int>(r[featureIndex]);
  return record_feature <= v ? leftChild->predict(r) : rightChild->predict(r);
}