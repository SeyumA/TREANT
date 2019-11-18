//
// Created by dg on 05/11/19.
//

#ifndef TREEANT_BININTNODE_H
#define TREEANT_BININTNODE_H

#include "nodes/INode.h"

/**
 * Binary integer node (partition in two parts depending on an integer value)
 */
class BinIntNode : public INode {
public:
  explicit BinIntNode(int featureIndex, int v, INode *leftChild,
                      INode *rightChild);

  ~BinIntNode() override;

  //[[nodiscard]] int predict(const record_t& r) const override;

  [[nodiscard]] std::vector<INode*> getChildren() const override;
  void setChild(std::size_t index, INode* newNodePtr) override;
  void setFeatureIndex(std::size_t index) override;

private:
  int featureIndex_;
  int v;
  INode *leftChild = nullptr;
  INode *rightChild = nullptr;
};

#endif // TREEANT_BININTNODE_H
