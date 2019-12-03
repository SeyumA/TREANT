//
// Created by dg on 25/11/19.
//

#include "nodes/INode.h"

INode::~INode() {
  for (auto &child : children_) {
    delete child;
    child = nullptr;
  }
}

INode::INode(index_t featureIndex) : featureIndex_(featureIndex) {}

INode::INode(index_t featureIndex, std::initializer_list<INode *> list)
    : featureIndex_(featureIndex), children_(list) {}
