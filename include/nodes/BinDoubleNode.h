//
// Created by dg on 25/11/19.
//

#ifndef TREEANT_BINDOUBLENODE_H
#define TREEANT_BINDOUBLENODE_H

#include <stdexcept>
#include "utils.h"
#include "nodes/INode.h"

class Dataset;

class BinDoubleNode : public INode {
public:
  explicit BinDoubleNode(index_t featureIndex, double_feature_t v);

  // This function is useful for building a decision tree.
  [[nodiscard]] partitions_t
  split(const partition_t &validIndexes,
        const feature_vector_t &featureVector) const override;

  // This function is useful to use the decision tree. It should call
  // recursively predict on the next child.
  [[nodiscard]] label_t predict(const record_t &record) const override;

private:
  double_feature_t v_;
};

#endif // TREEANT_BINDOUBLENODE_H
