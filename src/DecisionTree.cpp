//
// Created by dg on 01/11/19.
//

#include <functional>
#include <numeric>
#include <stack>
#include <visitors/GiniVisitor.h>

#include "DecisionTree.h"
#include "utils.h"

DecisionTree::DecisionTree(const Dataset &dataset, const std::size_t &maxDepth,
                           VisitorConstructorTypes visitorType) {
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

std::size_t DecisionTree::getHeight() const { return height_; }

label_t DecisionTree::predict(const record_t &record) const {
  return root_->predict(record);
}

DecisionTree::~DecisionTree() {
  delete root_;
  root_ = nullptr;
}

std::ostream &operator<<(std::ostream &os, const DecisionTree &dt) {
  // Recursive lambda for a depth first search visit
  std::function<std::string(INode *, std::string &, std::size_t)> treeAsString =
      [&treeAsString](INode *nToVisit, std::string &s, std::size_t lev) {
        // Append the current node taking into account its depth
        for (std::size_t i = 0; i < lev; i++) {
          s.append("\t");
        }
        s.append(nToVisit->stringify() + '\n');
        // Recursive call (DFS)
        for (const auto& child : nToVisit->children_) {
          treeAsString(child, s, lev + 1);
        }
        //
        return s;
      };

  std::string s;
  return os << treeAsString(dt.root_, s, 0);
}
