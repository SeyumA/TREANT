//
// Created by dg on 01/11/19.
//

#ifndef TREEANT_DECISIONTREE_H
#define TREEANT_DECISIONTREE_H

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
  static std::pair<INode *, std::size_t>
  buildRecursively(const Dataset &dataset,
                   const std::vector<index_t>& validIndexes,
                   const std::size_t &maxHeight,
                   const std::size_t &currDepth);

private:
  unsigned int height_;
  INode *root_ = nullptr;
};

#endif // TREEANT_DECISIONTREE_H
