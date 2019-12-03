//
// Created by dg on 01/11/19.
//

#include <numeric>
#include <stack>
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
      utils::buildRecursively(dataset, maxDepth, 1, visitor);
  // Get the important variables to build the decision tree.
  root_ = treeRoot;
  height_ = treeHeight;
  // Do not forget to delete the visitor
  delete visitor;
}

std::size_t DecisionTree::getHeight() const {
  return height_;
}

label_t DecisionTree::predict(const record_t &record) const {
  return root_->predict(record);
}

DecisionTree::~DecisionTree() {
  delete root_;
  root_ = nullptr;
}

std::ostream &operator<<(std::ostream &os, const DecisionTree &dt) {
  // Static lambda
  static auto printTabs = [&os](std::size_t n) {
    for (std::size_t i = 0; i < n; i++) {
      os << '\t';
    }
  };
  // Stack of the node pointers
  // Stack of the node levels (we need two stack in order for pre-visit)
  std::stack<std::pair<INode*, std::size_t>> nodePtrStack;
  nodePtrStack.push({dt.root_, 0});
  //
  while (!nodePtrStack.empty()) {
    // Pop from the  node pointer stack
    const auto [currNode, currLevel] = nodePtrStack.top();
    nodePtrStack.pop();
    // Actual printing of the current node with its spaces
    printTabs(currLevel);
    os << currNode->print() << std::endl;
    // Push the children in the two stacks
    for (const auto& child : currNode->children_) {
      nodePtrStack.push({child, currLevel + 1});
    }
  }
  //
  return os;
}
