//
// Created by dg on 06/11/19.
//

#ifndef TREEANT_BOOLEANNODE_H
#define TREEANT_BOOLEANNODE_H

#include <stdexcept>
#include "INode.h"

class BooleanNode : public INode {
public:
  // Constructors
  explicit BooleanNode(index_t featureIndex);

  // This function is useful for building a decision tree.
  [[nodiscard]] partitions_t
  split(const partition_t &validIndexes,
        const feature_vector_t &featureVector) const override;

  // This function is useful to use the decision tree. It should call
  // recursively predict on the next child.
  [[nodiscard]] label_t predict(const record_t &record) const override;

private:
  // Functions
  [[nodiscard]] std::string stringify() const override;
};

#endif // TREEANT_BOOLEANNODE_H
