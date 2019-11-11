//
// Created by dg on 05/11/19.
//

#ifndef TREEANT_INODE_H
#define TREEANT_INODE_H

#include <string>
#include <vector>

#include "types.h"

class INode {
public:
  virtual ~INode() = default;
  [[nodiscard]] virtual int predict(const record_t& r) const = 0;

  [[nodiscard]] virtual std::vector<INode*> getChildren() const = 0;
  virtual void setChild(std::size_t index, INode* newNodePtr) = 0;
  virtual void setFeatureIndex(std::size_t index) = 0;
};

#endif // TREEANT_INODE_H
