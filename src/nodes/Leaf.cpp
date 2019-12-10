//
// Created by dg on 05/11/19.
//

#include <sstream>
#include "nodes/Leaf.h"

Leaf::Leaf(int label) : INode(0), label_(label) {}

partitions_t Leaf::split(const partition_t &validIndexes,
                         const feature_vector_t &) const {
  partitions_t p = {validIndexes};
  return p;
}

std::string Leaf::stringify() const {
  std::ostringstream oss;
  oss << "Leaf(label = " << label_ << ")";
  return oss.str();
}
