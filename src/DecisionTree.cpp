//
// Created by dg on 01/11/19.
//

#include <functional>
#include <numeric>
#include <stack>
#include <visitors/GiniVisitor.h>

#include "DecisionTree.h"
#include "Node.h"
#include "utils.h"

DecisionTree::DecisionTree(const Dataset &dataset, const std::size_t &maxDepth,
                           VisitorConstructorTypes visitorType)
    : maxDepth_(maxDepth) {
  // Preliminary checks
  if (dataset.empty()) {
    std::runtime_error("Cannot build a decision tree from an empty dataset");
  }
  //  // Build the vector of indexes, at the beginning are all
  //  std::vector<index_t> indexes(dataset.size());
  //  std::iota(indexes.begin(), indexes.end(), 0);
  //  // Can be a parameter of the class or an argument of the constructor
  //  IFeatureVectorVisitor *visitor = nullptr;
  //  switch (visitorType) {
  //  case VisitorConstructorTypes::GINI:
  //    visitor = new GiniVisitor(indexes, dataset.getLabels());
  //    break;
  //  default:
  //    throw std::runtime_error("Invalid visitorType in DecisionTree
  //    constructor");
  //  }
  //  // Build the tree
  //  const auto [treeRoot, treeHeight] =
  //      utils::buildRecursively(dataset, maxDepth, 1, visitor);
  //  // Get the important variables to build the decision tree.
  //  root_ = treeRoot;
  //  height_ = treeHeight;
  //  // Do not forget to delete the visitor avoiding memory leaks
  //  delete visitor;
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
  std::function<std::string(Node *, std::string &, std::size_t)> treeAsString =
      [&treeAsString](Node *nToVisit, std::string &s, std::size_t lev) {
        // Append the current node taking into account its depth
        for (std::size_t i = 0; i < lev; i++) {
          s.append("\t");
        }
        s.append(nToVisit->stringify() + '\n');
        // Recursive call (DFS)
        treeAsString(nToVisit->left_, s, lev + 1);
        treeAsString(nToVisit->right_, s, lev + 1);
        //
        return s;
      };

  std::string s;
  return os << treeAsString(dt.root_, s, 0);
}

void DecisionTree::fit(const Dataset &dataset) {

  if (dataset.empty()) {
    throw std::runtime_error("ERROR DecisionTree::fit: Invalid "
                             "input data (empty dataset)");
  }
  // Prepare all in order get the root

  // - count the equal labels in the dataset

  // TODO: take a portion of the whole dataset

  // - get the rows in order to build the subset

  // - suggest a prediction for the root
  // (should be done inside fitRecursively, not? If we already reach the depth
  // limit we just stop)

  std::vector<std::size_t> rows(dataset.size());
  std::iota(rows.begin(), rows.end(), 0);
  root_ = fitRecursively(dataset, rows, std::vector<bool>(), 0);

  isTrained_ = true;
}

Node *DecisionTree::fitRecursively(const Dataset &dataset,
                                   const std::vector<std::size_t> &rows,
                                   const std::vector<bool> &blackList,
                                   std::size_t currHeight) {

  if (dataset.empty()) {
    throw std::runtime_error("ERROR DecisionTree::fitRecursively: Invalid "
                             "input data (empty dataset)");
  }

  if (currHeight > maxDepth_) {
    return nullptr;
  }

  // TODO: find the best split with the optimizer

  // Base case, the recursion is just started
  if (blackList.empty()) {

    // Return the mean of y
    const auto labels = dataset.getLabels();
    std::size_t countFalse = 0;
    std::size_t countTrue = 0;
    for (const auto &label : labels) {
      if (!label) {
        countFalse++;
      } else {
        countTrue++;
      }
    }
    auto toBeReturned = new Node(countFalse < countTrue);

    std::vector<bool> blackListLeft(dataset.size());

    toBeReturned.left = fitRecursively()
  }

  // Calculate the best split of the subset given by rows

  // Create the new node

  // If all the labels are already clustered do NOT do the recursive call

  return nullptr;
}
