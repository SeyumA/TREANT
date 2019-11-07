//
// Created by dg on 05/11/19.
//

#ifndef TREEANT_LEAF_H
#define TREEANT_LEAF_H

#include "INode.h"

class Leaf : public INode {
public:
  // Constructors
  Leaf() = delete;
  Leaf(Leaf&) = delete;
  Leaf(Leaf&&) = delete;
  explicit Leaf(int label);

  // Functions
  [[nodiscard]] int predict(const record_t&) const override;

  [[nodiscard]] std::vector<INode*> getChildren() const override;
  void setChild(std::size_t index, INode* newNodePtr) override;

private:
  int label;
};


#endif // TREEANT_LEAF_H
