//
// Created by dg on 05/11/19.
//

#ifndef TREEANT_INODE_H
#define TREEANT_INODE_H

#include "types.h"
#include <vector>

class Database;

class INode {
public:
  virtual ~INode() = default;

  [[nodiscard]] virtual std::vector<INode *> getChildren() const = 0;
  virtual void setChild(std::size_t index, INode *newNodePtr) = 0;
  virtual void setFeatureIndex(std::size_t index) = 0;

  // This function is useful for building a decision tree.
  virtual std::vector<std::vector<index_t>>
  split(const std::vector<index_t> &validIndexes,
        const feature_vector_t &featureVector) = 0;

  // This function is useful to use the decision tree. It should call
  // recursively predict on the next child.
  virtual label_t predict(const record_t &record) const = 0;
};

#endif // TREEANT_INODE_H
