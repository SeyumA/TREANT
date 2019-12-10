//
// Created by dg on 05/11/19.
//

#ifndef TREEANT_INODE_H
#define TREEANT_INODE_H

#include "types.h"
#include "utils.h"
#include <vector>

class DecisionTree;

class INode {
public:
  explicit INode(index_t featureIndex);
  explicit INode(index_t, std::initializer_list<INode *>);

  virtual ~INode();

  // This function is useful for building a decision tree.
  [[nodiscard]] virtual partitions_t
  split(const partition_t &validIndexes,
        const feature_vector_t &featureVector) const = 0;

  // This function is useful to use the decision tree. It should call
  // recursively predict on the next child.
  [[nodiscard]] virtual label_t predict(const record_t &record) const = 0;

  // Visitors that are friend of this class
  friend std::pair<INode *, std::size_t>
  utils::buildRecursively(const Dataset &dataset, const std::size_t &maxHeight,
                          const std::size_t &currDepth,
                          IFeatureVectorVisitor *visitor);

  friend std::ostream &operator<<(std::ostream &os, const DecisionTree &dt);

protected:
  index_t featureIndex_;
  std::vector<INode *> children_;

private:
  [[nodiscard]] virtual std::string stringify() const = 0;
};

#endif // TREEANT_INODE_H
