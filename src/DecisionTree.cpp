//
// Created by dg on 01/11/19.
//

#include "DecisionTree.h"
#include "splitters/DummySplitter.h"

DecisionTree::DecisionTree(const dataset_t &dataset,
                           const std::size_t &maxDepth)
    : depth(0), root(nullptr), splitter(std::make_shared<DummySplitter>()){
  // Build the vector of indexes, at the beginning are all
  std::vector<unsigned long> indexes(dataset.size());
  for (unsigned long i = 0; i < indexes.size(); i++) {
    indexes[i] = i;
  }

  auto [curr_root, curr_depth] =
      buildRecursively(dataset, indexes, splitter.get(), 0, maxDepth);
  root = curr_root;
  depth = curr_depth;
}

DecisionTree::~DecisionTree() {
  delete root;
  root = nullptr;
}

/**
 * Recursively builds a decision tree
 * @param dataset the original dataset
 * @param subset is a vector of indexes that picks a subset of the dataset
 * @param maxDepth the maximum depth of the resulting tree
 * @return a pair containing the tree root and the tree depth
 */
std::pair<INode *, std::size_t> DecisionTree::buildRecursively(
    const dataset_t &dataset, const std::vector<unsigned long> &subset,
    const ISplitter *splitter, const std::size_t &currDepth,
    const std::size_t &maxDepth) {

  // Base cases where the function should stop

  // Find the best split
  splitter->split(dataset, subset);
  // TODO: decide if we continue splitting or not
  if (currDepth == maxDepth) {
    // TODO: continue from here
  }

  return std::make_pair(nullptr, 0);
}

int DecisionTree::predict(const record_t &r) const { return root->predict(r); }
