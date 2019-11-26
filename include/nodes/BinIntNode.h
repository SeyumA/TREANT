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

  // utils::buildRecursively must have access to the children
  friend std::pair<INode *, std::size_t> utils::buildRecursively(
      const Dataset &dataset, const std::vector<index_t> &validIndexes,
      const std::size_t &maxHeight, const std::size_t &callerDepth);

private:
  int_feature_t v_;
};

#endif // TREEANT_BININTNODE_H
