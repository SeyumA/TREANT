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
  // Destructor
  ~Leaf();

  // Functions
  //[[nodiscard]] int predict(const record_t&) const override;

  [[nodiscard]] std::vector<INode *> getChildren() const override;
  void setChild(std::size_t index, INode *newNodePtr) override;
  void setFeatureIndex(std::size_t index) override;

  // This function is useful for building a decision tree.
  [[nodiscard]] std::vector<std::vector<index_t>>
  split(const std::vector<index_t> &validIndexes,
        const feature_vector_t &featureVector) override {
    std::vector<std::vector<index_t>> p;
    return p;
  }

  // This function is useful to use the decision tree. It should call
  // recursively predict on the next child.
  [[nodiscard]] label_t predict(const record_t &) const override {
    return label;
  }

private:
  std::int32_t label;
};

#endif // TREEANT_LEAF_H
