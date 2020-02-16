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

void DecisionTree::fit(const Dataset &dataset, int budget,
                       SplitOptimizer::Impurity impurityType) {

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
  std::vector<int> costs(dataset.size(), budget);
  root_ = fitRecursively(dataset, rows, std::vector<bool>(), 1, Attacker(),
                         costs, impurityType);

  // ...

  isTrained_ = true;
}

Node *DecisionTree::fitRecursively(const Dataset &dataset,
                                   const indexes_t &rows,
                                   const indexes_t &validFeatures,
                                   std::size_t currHeight,
                                   const Attacker &attacker,
                                   const std::vector<int> &costs,
                                   SplitOptimizer::Impurity impurityType) {

  if (dataset.empty() || rows.empty()) {
    throw std::runtime_error("ERROR DecisionTree::fitRecursively: Invalid "
                             "input data (empty dataset)");
  }

  // First base case
  if (currHeight > maxDepth_) {
    return nullptr;
  }

  // Count true and false, could lead to a base case (see below)
  const auto &labels = dataset.getLabels();
  std::size_t countFalse = 0;
  std::size_t countTrue = 0;
  for (const auto &row : rows) {
    if (!labels[row]) {
      countFalse++;
    } else {
      countTrue++;
    }
  }

  // Other base cases
  if (currHeight == maxDepth_) {
    // Returns false if the majority of the labels is false otherwise true,
    // tie case -> false.
    return new Node(countFalse < countTrue);
  }
  // If all the labels are already clustered do NOT do the recursive call
  else if (!countTrue) {
    return new Node(false);
  } else if (!countFalse) {
    return new Node(true);
  }
  // TODO: if you put the minimum number of instances per node and the instances
  //       falling in this node are less than this number then we have another
  //       base case (see base case 3 in parallel_robust_forest.py)

  // We recreate the optimizer at each node for concurrency reasons
  auto splitOptimizer = SplitOptimizer(impurityType);
  // Get the best split
  gain_t gain;
  index_t bestSplitFeatureId;
  feature_t bestSplitValue;
  indexes_t rowsLeft;
  indexes_t rowsRight;
  // Find the best split with the optimizer
  std::tie(gain, bestSplitFeatureId, bestSplitValue, rowsLeft, rowsRight) =
      splitOptimizer.optimizeGain(dataset, rows, blackList, attacker, costs);

  if (gain > 0.0) {
    // Build the node to be returned
    auto toBeReturned = new Node(bestSplitFeatureId, bestSplitValue);
    //
    // Prepare for the recursive step
    // Build the blackListLeft
    std::vector<bool> blackListDownstream(blackList);
    if (blackListDownstream.empty()) {
      blackListDownstream.resize(dataset.size(), false);
    }
    blackListDownstream[bestSplitFeatureId] = true;

    // TODO: Update the costs
    std::vector<int> costsLeft(costs);
    costsLeft[bestSplitFeatureId] -= 10; // this must be modified
    std::vector<int> costsRight(costs);
    costsRight[bestSplitFeatureId] -= 10; // this must be modified

    // TODO: update constraints
    //       constraints ... maybe from the optimizeGain

    // Set the left node
    Node *leftNode =
        fitRecursively(dataset, rowsLeft, blackListDownstream, currHeight + 1,
                       attacker, costsLeft, impurityType);
    toBeReturned->setLeft(leftNode);
    // Set the right node
    Node *rightNode =
        fitRecursively(dataset, rowsRight, blackListDownstream, currHeight + 1,
                       attacker, costsRight, impurityType);
    toBeReturned->setRight(rightNode);

    return toBeReturned;

  } else {
    return new Node(countFalse < countTrue);
  }
}
