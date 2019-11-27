//
// Created by dg on 01/11/19.
//

#include <nodes/Leaf.h>
#include <numeric>
#include <visitors/GiniVisitor.h>

#include "DecisionTree.h"
#include "utils.h"

DecisionTree::DecisionTree(const Dataset &dataset, const std::size_t &maxDepth,
                           VisitorConstructorTypes visitorType) {
  //
  // Preliminary checks
  if (dataset.empty()) {
    std::runtime_error("Cannot build a decision tree from an empty dataset");
  }
  // Build the vector of indexes, at the beginning are all
  std::vector<index_t> indexes(dataset.size());
  std::iota(indexes.begin(), indexes.end(), 0);
  // Can be a parameter of the class or an argument of the constructor
  IFeatureVectorVisitor *visitor = nullptr;
  switch (visitorType) {
  case VisitorConstructorTypes::GINI:
    visitor = new GiniVisitor(indexes, dataset.getLabels());
    break;
  default:
    throw std::runtime_error("Invalid visitorType in DecisionTree constructor");
  }

  // Build the tree
  const auto [treeRoot, treeHeight] =
      utils::buildRecursively(dataset, indexes, maxDepth, 0, visitor);
  // Get the important variables to build the decision tree.
  root_ = treeRoot;
  height_ = treeHeight;
  //
  delete visitor;
}

label_t DecisionTree::predict(const record_t &record) const {
  return root_->predict(record);
}

DecisionTree::~DecisionTree() {
  delete root_;
  root_ = nullptr;
}
