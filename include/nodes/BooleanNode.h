//
// Created by dg on 06/11/19.
//

#ifndef TREEANT_BOOLEANNODE_H
#define TREEANT_BOOLEANNODE_H

#include "INode.h"

class BooleanNode : public INode {
public:
  // Constructors
  BooleanNode() = delete;
  BooleanNode(BooleanNode&) = delete;
  BooleanNode(BooleanNode&&) = delete;
  explicit BooleanNode(unsigned featureIndex) : featureIndex(featureIndex) {}
  // Destructor
  ~BooleanNode() override;

  [[nodiscard]] int predict(const record_t &r) const override;

  [[nodiscard]] std::vector<INode *> getChildren() const override;
  void setChild(std::size_t index, INode *newNodePtr) override;

private:
  unsigned featureIndex;
  INode *leftChild = nullptr;
  INode *rightChild = nullptr;
};

#endif // TREEANT_BOOLEANNODE_H
