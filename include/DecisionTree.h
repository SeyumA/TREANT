//
// Created by dg on 01/11/19.
//

#ifndef TREEANT_DECISIONTREE_H
#define TREEANT_DECISIONTREE_H

#include <memory> // shared_ptr
#include <vector>

#include "nodes/INode.h"
#include "splitters/ISplitter.h"
#include "Dataset.h"

class ISplitter;

class DecisionTree final {

public:
  // Constructors
  DecisionTree() = delete;
  DecisionTree(DecisionTree&) = delete;
  DecisionTree(DecisionTree&&) = delete;

  explicit DecisionTree(const Dataset &dataset, const std::size_t &maxDepth);

  // Destructor
  ~DecisionTree();

  // Functions
  static std::pair<INode *, std::size_t>
  buildRecursively(const DataSubset &dataSubset,
                   const std::size_t &maxHeight,
                   const std::size_t &currDepth);

  [[nodiscard]] int predict(const record_t &r) const;

private:
  unsigned int height_;
  INode *root_ = nullptr;
};

#endif // TREEANT_DECISIONTREE_H
