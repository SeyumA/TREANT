//
// Created by dg on 05/11/19.
//

#ifndef TREEANT_LEAF_H
#define TREEANT_LEAF_H

#include "INode.h"

class Leaf : public INode {
public:
  // Constructors
  explicit Leaf(int label);

  // This function is useful for building a decision tree.
  [[nodiscard]] partitions_t
  split(const partition_t  &validIndexes,
        const feature_vector_t &) const override;

  // This function is supposed to be used inside a decision tree.
  // It should call recursively predict on the next child.
  [[nodiscard]] label_t predict(const record_t &) const override {
    return label_;
  }

private:
  label_t label_;

  // Private functions
  [[nodiscard]] std::string stringify() const override;
};

#endif // TREEANT_LEAF_H
