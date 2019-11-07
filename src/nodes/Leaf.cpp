//
// Created by dg on 05/11/19.
//

// STD library
#include <stdexcept>
// Local headers
#include "nodes/Leaf.h"

Leaf::Leaf(int label) : label(label) {}

int Leaf::predict(const record_t &) const { return label; }

std::vector<INode *> Leaf::getChildren() const {
  throw std::runtime_error("Leaf::getChildren() error: Leaf node has no child");
}

void Leaf::setChild(std::size_t, INode *) {
  throw std::runtime_error("Leaf::setChild() error: Leaf node has no child");
}
