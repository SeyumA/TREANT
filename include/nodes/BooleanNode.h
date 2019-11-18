//
// Created by dg on 06/11/19.
//

#ifndef TREEANT_BOOLEANNODE_H
#define TREEANT_BOOLEANNODE_H

#include "INode.h"

class BooleanNode : public INode {
public:
  // Constructors
  explicit BooleanNode(std::size_t featureIndex);
  // Destructor
  ~BooleanNode() override;

//  [[nodiscard]] int predict(const record_t &r) const override;

  [[nodiscard]] std::vector<INode *> getChildren() const override;
  void setChild(std::size_t index, INode *newNodePtr) override;
  void setFeatureIndex(std::size_t index) override;

private:
  unsigned featureIndex_;
  INode *leftChild = nullptr;
  INode *rightChild = nullptr;
};

#endif // TREEANT_BOOLEANNODE_H
