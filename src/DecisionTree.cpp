//
// Created by dg on 01/11/19.
//

#include <nodes/Leaf.h>
#include <numeric>

#include "DecisionTree.h"
#include "splitters/DummySplitter.h"

DecisionTree::DecisionTree(const Dataset &dataset,
                           const std::size_t &maxDepth) {
  //
  // Preliminary checks
  if (!dataset.size()) {
    std::runtime_error("Cannot build a decision tree from an empty dataset");
  }
  // Build the vector of indexes, at the beginning are all
  DataSubset dataSubset(dataset);

  const auto [treeRoot, treeHeight] = buildRecursively(dataSubset, maxDepth, 0);
  root_ = treeRoot;
  height_ = treeHeight;
}

DecisionTree::~DecisionTree() {
  delete root_;
  root_ = nullptr;
}

/**
 * Recursively builds a decision tree
 * @param dataset the original dataset
 * @param subset is a vector of indexes that picks a subset of the dataset
 * @param splitter the ISplitter used to find the best split for this tree
 * @param maxDepth the maximum depth of the resulting tree
 * @param callerDepth the depth of the caller node
 * @return a pair containing the tree root and the height of the tree rooted by
 * the first output
 */
std::pair<INode *, std::size_t>
DecisionTree::buildRecursively(const DataSubset &dataSubset,
                               const std::size_t &maxHeight,
                               const std::size_t &callerDepth) {
  //
  // Find the best split and continue building the tree
  if (callerDepth >= maxHeight) {
    throw std::runtime_error("callerDepth must be less than maxHeight");
  } else if (callerDepth + 1 == maxHeight) {
    // A leaf should be returned with the mostPopular label in the subset
    // TODO: implement this
    auto mostPopularLabel = dataSubset.getMostFrequentLabel();
    return std::make_pair(new Leaf(mostPopularLabel), 1);
  } else {
    // TODO implement the recursive call




    return std::make_pair(nullptr, 0);
  }
}

int DecisionTree::predict(const record_t &r) const { return root_->predict(r); }
