//
// Created by dg on 05/11/19.
//

#ifndef TREEANT_BININTNODE_H
#define TREEANT_BININTNODE_H

#include "nodes/INode.h"
#include "utils.h"
#include <stdexcept>

class Dataset;

/**
 * Binary integer node (partition in two parts depending on an integer value)
 */
class BinIntNode : public INode {
public:
  explicit BinIntNode(index_t featureIndex, int_feature_t v);

  // This function is useful for building a decision tree.
  [[nodiscard]] partitions_t
  split(const partition_t &validIndexes,
        const feature_vector_t &featureVector) const override;

  // This function is useful to use the decision tree. It should call
  // recursively predict on the next child.
  [[nodiscard]] label_t predict(const record_t &record) const override ;

private:
  int_feature_t v_;
  // Functions
  [[nodiscard]] std::string stringify() const override;
};

#endif // TREEANT_BININTNODE_H
