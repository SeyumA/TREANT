//
// Created by dg on 01/11/19.
//

#include <nodes/Leaf.h>
#include <numeric>

#include "DecisionTree.h"
#include "splitters/DummySplitter.h"
#include "nodes/BooleanNode.h"
#include "nodes/BinIntNode.h"
#include "nodes/BinDoubleNode.h"

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
      buildRecursively(dataset, indexes, maxDepth, 0);
  root_ = treeRoot;
  height_ = treeHeight;
}

DecisionTree::~DecisionTree() {
  delete root_;
  root_ = nullptr;
}

std::pair<INode *, std::size_t> DecisionTree::buildRecursively(
    const Dataset &dataset, const std::vector<index_t> &validIndexes,
    const std::size_t &maxHeight, const std::size_t &callerDepth) {
  //
  // Find the best split and continue building the tree
  if (callerDepth >= maxHeight) {
    throw std::runtime_error("callerDepth must be less than maxHeight");
  } else if (callerDepth + 1 == maxHeight) {
    // A leaf should be returned with the mostPopular label in the subset
    return std::make_pair(
        new Leaf(dataset.getMostFrequentLabel(validIndexes).first), 1);
  } else if (const auto [l, f] = dataset.getMostFrequentLabel(validIndexes);
             f == dataset.size()) {
    return std::make_pair(new Leaf(l), 1);
  } else {
    // Assuming that there are different labels in the current training subset
    const auto &featureColumns = dataset.getFeatureColumns();

    // Build the column visitor
    // TODO: pass the visitor as const reference to the buildRecursively
    // function
    double impurity = 0.0;
    INode* candidate = nullptr;
    std::size_t featureIndex = 0;

    // TODO: This visitor should be become a friend class of every touched node
    const auto visitor = [&](auto&& arg) {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, bool_vector_t>) {
        impurity = 1.0;
        candidate = new BooleanNode(featureIndex);
      }
      else if constexpr (std::is_same_v<T, int_vector_t>) {
        impurity = 2.0;
        candidate = new BinIntNode(featureIndex, 0, nullptr, nullptr);
      }
      else if constexpr (std::is_same_v<T, double_vector_t>) {
        impurity = 3.0;
        // TODO: create a DoubleNode
        candidate = new BinDoubleNode(featureIndex, 0.0, nullptr, nullptr);
      }
      else {
        throw std::runtime_error("non-exhaustive visitor!");
      }
    };

    double lowestImpurity = -1.0;
    INode* bestCandidate = nullptr;
    for (std::size_t i = 0; i < featureColumns.size(); i++) {
      featureIndex = i;
      std::visit(visitor, featureColumns[i]);
      // impurity has the new value
      if (lowestImpurity < 0.0) {
        lowestImpurity = impurity;
      } else if (impurity < lowestImpurity) {
        lowestImpurity = impurity;
        bestCandidate = candidate;
      } else {
        delete candidate;
        candidate = nullptr;
      }
    }
    // Build partitions
    std::vector<std::vector<index_t>> partitions;
    //
    std::size_t maxDepth = 0;
    for (std::size_t i = 0; i < candidate->getChildren().size(); ++i) {
      auto [child, h] = buildRecursively(dataset, partitions[i], maxHeight, callerDepth + 1);
      candidate->setChild(i, child);
      maxDepth = maxDepth < h ? h : maxDepth;
    }
    //
    return std::make_pair(bestCandidate, maxDepth);
  }
}
