//
// Created by dg on 01/11/19.
//

#include "DecisionTree.h"
#include "splitters/DummySplitter.h"

DecisionTree::DecisionTree(const dataset_t &dataset,
                           const std::size_t &maxDepth)
    : height_(0), root_(nullptr), splitter_(std::make_shared<DummySplitter>()) {
  // Build the vector of indexes, at the beginning are all
  std::vector<unsigned long> indexes(dataset.size());
  for (unsigned long i = 0; i < indexes.size(); i++) {
    indexes[i] = i;
  }

  std::size_t treeDepth = 0;
  const auto [treeRoot, treeHeight] =
      buildRecursively(dataset, indexes, splitter_.get(), maxDepth, treeDepth);
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
std::pair<INode *, std::size_t> DecisionTree::buildRecursively(
    const dataset_t &dataset, const std::vector<unsigned long> &subset,
    const ISplitter *splitter, const std::size_t &maxHeight,
    const std::size_t &callerDepth) {

  // Find the best split and continue building the tree
  if (callerDepth >= maxHeight) {
    throw std::runtime_error("callerDepth must be < maxHeight");
  } else if (callerDepth + 1 == maxHeight) {
    // A leaf should be returned
    return std::make_pair(splitter->split(dataset, subset, true).first, 1);
  } else {
    const auto [splitterNode, partitions] =
        splitter->split(dataset, subset, false);
    // recursive call
    std::size_t maxChildrenHeight = 0;
    for (std::size_t i = 0; i < partitions.size(); i++) {
      const auto& p = partitions[i];
      // TODO: continue from here
      auto [newChild, childHeight] =
          buildRecursively(dataset, p, splitter, maxHeight, callerDepth + 1);
      splitterNode->setChild(i, newChild);
      maxChildrenHeight = std::max(maxChildrenHeight, childHeight);
    }
    //
    return std::make_pair(splitterNode, maxChildrenHeight);
  }
}

int DecisionTree::predict(const record_t &r) const { return root_->predict(r); }
