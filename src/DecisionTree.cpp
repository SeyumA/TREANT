//
// Created by dg on 01/11/19.
//

#include <nodes/Leaf.h>
#include <numeric>

#include "DecisionTree.h"
#include "utils.h"

DecisionTree::DecisionTree(const Dataset &dataset,
                           const std::size_t &maxDepth) {
  //
  // Preliminary checks
  if (dataset.empty()) {
    std::runtime_error("Cannot build a decision tree from an empty dataset");
  }
  // Build the vector of indexes, at the beginning are all
  std::vector<index_t> indexes(dataset.size());
  std::iota(indexes.begin(), indexes.end(), 0);
  // Build the tree
  const auto [treeRoot, treeHeight] =
      utils::buildRecursively(dataset, indexes, maxDepth, 0);
  root_ = treeRoot;
  height_ = treeHeight;
}

DecisionTree::~DecisionTree() {
  delete root_;
  root_ = nullptr;
}
