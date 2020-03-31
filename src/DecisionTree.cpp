//
// Created by dg on 01/11/19.
//

#include <functional>
#include <numeric>
#include <stack>

#include "Attacker.h"
#include "Constraint.h"
#include "Dataset.h"
#include "DecisionTree.h"
#include "Node.h"
#include "SplitOptimizer.h"
#include "utils.h"

DecisionTree::DecisionTree(std::size_t maxDepth) : maxDepth_(maxDepth) {}

DecisionTree::DecisionTree(std::size_t maxDepth, bool isAffine)
    : maxDepth_(maxDepth), isAffine_(isAffine) {}

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
        // Pre-order visit
        if (nToVisit) {
          // Append the current node taking into account its depth
          for (std::size_t i = 0; i < lev; i++) {
            s.append("\t");
          }
          s.append(nToVisit->stringify() + '\n');
          if (nToVisit->left_ && nToVisit->right_) {
            treeAsString(nToVisit->left_, s, lev + 1);
            treeAsString(nToVisit->right_, s, lev + 1);
          } else if (nToVisit->left_) {
            treeAsString(nToVisit->left_, s, lev + 1);
          } else if (nToVisit->right_) {
            treeAsString(nToVisit->right_, s, lev + 1);
          }
        }
        //
        return s;
      };

  std::string s;
  return os << treeAsString(dt.root_, s, 0);
}

void DecisionTree::fit(const Dataset &dataset, cost_t budget,
                       Impurity impurityType) {
  // Corner cases:
  if (dataset.empty()) {
    throw std::runtime_error("ERROR DecisionTree::fit: Invalid "
                             "input data (empty dataset)");
  }

  if (budget < 0) {
    throw std::runtime_error(
        "ERROR DecisionTree::fit: Invalid "
        "input data (budget must be positive or equal to zero)");
  }

  // At the beginning all the features and all the rows are active
  std::vector<std::size_t> rows(dataset.size());
  std::iota(rows.begin(), rows.end(), 0);
  std::vector<std::size_t> validFeatures(dataset.getFeatureColumns().size());
  std::iota(validFeatures.begin(), validFeatures.end(), 0);
  // At the beginning all the costs are equal to 0.0 (one for each row)
  std::unordered_map<index_t, cost_t> costs;
  for (index_t i = 0; i < rows.size(); i++) {
    costs[i] = 0.0;
  }

  // Empty constraints at the beginning
  std::vector<Constraint> constraints;
  // Calculate current prediction as the default
  prediction_t currentPrediction = dataset.getDefaultPrediction();

  std::string attackerFile =
      "/home/dg/source/repos/uni/treeant/data/attacks.json";
  Attacker attacker(dataset, attackerFile, budget);
  root_ = fitRecursively(dataset, rows, validFeatures, 0, attacker, costs,
                         currentPrediction, impurityType, constraints);

  // height_ is updated in the fitRecursively method
}

bool DecisionTree::isTrained() const { return root_ != nullptr; }

Node *DecisionTree::fitRecursively(
    const Dataset &dataset, const indexes_t &rows,
    const indexes_t &validFeatures, std::size_t currHeight,
    const Attacker &attacker, const std::unordered_map<index_t, cost_t> &costs,
    const prediction_t &nodePrediction, Impurity impurityType,
    const std::vector<Constraint> &constraints) {

  // As input there is a node prediction (floating point)
  // so this function always returns a new Node

  if (dataset.empty()) {
    throw std::runtime_error("ERROR DecisionTree::fitRecursively: Invalid "
                             "input data (empty dataset)");
  }

  // First base case
  if (currHeight > maxDepth_ || rows.empty()) {
    return nullptr;
  }

  // Base case: we reach the maxDepth limit
  // TODO: base case where all the labels are on one side,
  //  maybe this case does not exists because it a regression (fp labels)
  Node *ret = new Node(nodePrediction);
  // Weird passages but they are done in Python
  ret->setNodePrediction(nodePrediction);
  //  prediction_t currentPrediction = ret->getNodePrediction(); // rounded
  //  score
  prediction_t currentPredictionScore = ret->getNodePredictionScore();

  // We recreate the optimizer at each node for concurrency reasons
  const auto splitOptimizer = SplitOptimizer(impurityType);
  // Calculate the current score
  double currentScore =
      splitOptimizer.evaluateSplit(dataset, rows, currentPredictionScore);
  ret->setLossValue(currentScore);

  // Base case -> return a leaf
  if (currHeight == maxDepth_ || rows.size() < maxPerNode_) {
    // Returns false if the majority of the labels is false otherwise true,
    // tie case -> false.
    return ret;
  }

  // TODO: Consider another base case
  //       if you put the minimum number of instances per node and the instances
  //       falling in this node are less than this number then we have another
  //       base case (see base case 3 in parallel_robust_forest.py)

  // Get the best split (see line 1543 of parallel_robust_forest.py)
  gain_t bestGain = 0.0; // highest gain
  indexes_t bestSplitLeftFeatureId,
      bestSplitRightFeatureId; // indexes going left and right
  index_t bestSplitFeatureId;  // best feature splitter
  split_value_t bestSplitValue;
  split_value_t bestNextSplitValue;
  prediction_t bestPredLeft, bestPredRight;
  double bestSSEuma = 0.0;
  std::vector<Constraint> constraintsLeft, constraintsRight;
  std::unordered_map<index_t, cost_t> costsLeft, costsRight;

  // Find the best split with the optimizer
  bool optimizerSuccess = splitOptimizer.optimizeGain(
      dataset, rows, validFeatures, attacker, costs, constraints, currentScore,
      currentPredictionScore, bestGain, bestSplitLeftFeatureId,
      bestSplitRightFeatureId, bestSplitFeatureId, bestSplitValue,
      bestNextSplitValue, bestPredLeft, bestPredRight, bestSSEuma,
      constraintsLeft, constraintsRight, costsLeft, costsRight);

  if (optimizerSuccess) {
    // Build the node to be returned
    ret->setLossValue(bestSSEuma);
    ret->setGainValue(bestGain);
    ret->setBestSplitFeatureId(bestSplitFeatureId);
    ret->setBestSplitValue(bestSplitValue);

    //
    // Prepare for the recursive step
    // Build the validFeaturesDownstream if isAffine (see line 1646 python code)
    const indexes_t validFeaturesDownstream =
        !this->isAffine_ ? indexes_t(validFeatures.begin(), validFeatures.end())
                         : [&validFeatures, &bestSplitFeatureId]() {
                             indexes_t ret;
                             for (const auto &f : validFeatures) {
                               if (f != bestSplitFeatureId) {
                                 ret.emplace_back(f);
                               }
                             }
                             return ret;
                           }();
    // Set the left node
    Node *leftNode =
        fitRecursively(dataset, bestSplitLeftFeatureId, validFeaturesDownstream,
                       currHeight + 1, attacker, costsLeft, bestPredLeft,
                       impurityType, constraintsLeft);
    ret->setLeft(leftNode);
    // Set the right node
    Node *rightNode = fitRecursively(dataset, bestSplitRightFeatureId,
                                     validFeaturesDownstream, currHeight + 1,
                                     attacker, costsRight, bestPredRight,
                                     impurityType, constraintsRight);
    ret->setRight(rightNode);

    // Update the decision tree height if necessary
    if (height_ < currHeight) {
      height_ = currHeight;
    }
  }

  // TODO: set node constraint, they are used ... (where ???)
  //  (every node has a list of constraints that is empty in most of the test
  //  cases)

  return ret;
}
