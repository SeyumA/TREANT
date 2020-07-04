//
// Created by dg on 01/11/19.
//

#include <fstream>
#include <functional>
#include <numeric>
#include <regex>
#include <stack>

#include "Attacker.h"
#include "Constraint.h"
#include "Dataset.h"
#include "DecisionTree.h"
#include "Node.h"
#include "SplitOptimizer.h"
#include "utils.h"

DecisionTree::DecisionTree() : root_(nullptr) {}

void DecisionTree::load(const std::string &filePath) {

  static std::regex childrenRegex(R"__(^\[(\d{1,}),(\d{1,})\](.*))__");

  std::ifstream ifs;
  ifs.open(filePath);
  if (!ifs.is_open() || !ifs.good()) {
    throw std::runtime_error(
        "The decision tree file stream is not open or not good");
  }
  // Reading the first line and populate the list of column names
  std::string line;
  std::vector<Node *> nodePointers;
  // Get the nodes
  while (std::getline(ifs, line)) {

    //    std::cout << "line: '" << line << "'\n";
    std::cmatch cm; // same as std::match_results<const char*> cm;
    // using explicit flags:
    std::regex_match(line.c_str(), cm, childrenRegex);

    if (!cm.empty()) {
      // internal node
      const auto leftId = std::stoi(cm[1]);
      const auto rightId = std::stoi(cm[2]);
      nodePointers.emplace_back(new Node(cm[3]));
      nodePointers[nodePointers.size() - 1]->setLeft(nodePointers[leftId]);
      nodePointers[nodePointers.size() - 1]->setRight(nodePointers[rightId]);
    } else {
      // leaf
      nodePointers.emplace_back(new Node(line));
    }
  }

  if (nodePointers.empty()) {
    // No valid node present in the file
    delete root_;
    root_ = nullptr;
  }

  Node *root = nodePointers[nodePointers.size() - 1];
  // check count in the internal struct (no isolated nodes)
  if (nodePointers.size() != root->getSubtreeNumberNodes()) {
    for (auto &nPtr : nodePointers) {
      delete nPtr;
    }
    throw std::runtime_error("Some node is isolated (error in load " +
                             filePath + " file");
  }

  // delete the root and its children if needed (already trained)
  delete root_;
  // Update the root_
  root_ = root;
}

void DecisionTree::save(const std::string &filePath) const {
  std::ofstream ofs;
  ofs.open(filePath, std::ios::trunc);
  if (!ofs.is_open() || !ofs.good()) {
    throw std::runtime_error(
        "The decision tree file stream is not open or not good");
  }
  root_->getSubtreeStruct(ofs);
  ofs.close();
}

std::size_t DecisionTree::getHeight() const {
  if (!root_) {
    return 0;
  }
  return root_->getSubtreeHeight();
}

std::size_t DecisionTree::getNumberNodes() const {
  if (!root_) {
    return 0;
  }
  return root_->getSubtreeNumberNodes();
}

void DecisionTree::predict(const double *X, const unsigned rows,
                           const unsigned cols, double *res,
                           const bool isRowsWise, const bool score) const {
  if (!root_) {
    throw std::runtime_error(
        "The tree is not trained, prediction cannot be done");
  }

  // If the X matrix is row-wise, then we do not need extra memory
  if (isRowsWise) {
    std::size_t offset = 0;
    for (std::size_t i = 0; i < rows; i++) {
      res[i] = root_->predict(X + offset, score);
      offset += cols;
    }
  } else {
    // we need to allocate the record
    feature_t *record = (feature_t *)malloc(sizeof(feature_t) * cols);
    for (std::size_t i = 0; i < rows; i++) {
      // Update the record
      for (std::size_t j = 0; j < cols; j++) {
        record[j] = X[j * rows + i];
      }
      res[i] = root_->predict(record, score);
    }
    free((void *)record);
  }
}

DecisionTree::~DecisionTree() {
  if (root_) {
    delete root_;
    root_ = nullptr;
  }
}

bool DecisionTree::isTrained() const { return root_ != nullptr; }

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

void DecisionTree::fit(const Dataset &dataset, const std::string &attackerFile,
                       const cost_t &budget, const unsigned &threads,
                       const bool &useICML2019, const unsigned &maxDepth,
                       const unsigned minPerNode, const bool isAffine,
                       const Impurity impurityType) {

  if (threads < 1) {
    throw std::runtime_error(
        "Invalid threads parameter in fit function, it must be > 0");
  }

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
  std::vector<std::size_t> rows(dataset.rows_);
  std::iota(rows.begin(), rows.end(), 0);
  std::vector<std::size_t> validFeatures(dataset.cols_);
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

  Attacker attacker(dataset, attackerFile, budget);
  root_ = fitRecursively(dataset, rows, validFeatures, 0, attacker, costs,
                         currentPrediction, maxDepth, minPerNode, isAffine,
                         impurityType, constraints, threads, useICML2019);

  // height_ is updated in the fitRecursively method
}

Node *DecisionTree::fitRecursively(
    const Dataset &dataset, const indexes_t &rows,
    const indexes_t &validFeatures, std::size_t currHeight,
    const Attacker &attacker, const std::unordered_map<index_t, cost_t> &costs,
    const prediction_t &nodePrediction, const unsigned &maxDepth,
    const unsigned minPerNode, const bool isAffine, Impurity impurityType,
    const std::vector<Constraint> &constraints, const unsigned &numThreads,
    const bool &useICML2019) const {

  // As input there is a node prediction (floating point)
  // so this function always returns a new Node or null pointer (see first base
  // case)

  if (dataset.empty()) {
    throw std::runtime_error("ERROR DecisionTree::fitRecursively: Invalid "
                             "input data (empty dataset)");
  }

  // First base case
  if (currHeight > maxDepth || rows.empty()) {
    return nullptr;
  }

  // Base case where all the labels are on one side is automatically handled
  Node *ret = new Node(rows.size(), nodePrediction);
  // Weird passages but they are done in Python
  ret->setNodePrediction(nodePrediction);
  prediction_t currentPredictionScore = ret->getNodePredictionScore();
  // We recreate the optimizer at each node for concurrency reasons
  const auto splitOptimizer = SplitOptimizer(impurityType);
  // Calculate the current score
  double currentScore =
      splitOptimizer.evaluateSplit(dataset, rows, currentPredictionScore);
  ret->setLossValue(currentScore);

  // Base case -> return a leaf if max depth is reached or too few rows
  if (currHeight == maxDepth || rows.size() < minPerNode) {
    // Returns false if the majority of the labels is false otherwise true,
    // tie case -> false.
    return ret;
  }

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
      currentPredictionScore, numThreads, useICML2019, bestGain,
      bestSplitLeftFeatureId, bestSplitRightFeatureId, bestSplitFeatureId,
      bestSplitValue, bestNextSplitValue, bestPredLeft, bestPredRight,
      bestSSEuma, constraintsLeft, constraintsRight, costsLeft, costsRight);

  if (optimizerSuccess) {
    // Build the node to be returned
    ret->setLossValue(bestSSEuma);
    ret->setGainValue(bestGain);
    ret->setBestSplitFeatureId(bestSplitFeatureId);
    ret->setBestSplitValue(bestSplitValue);
    if (!dataset.isFeatureNumerical(bestSplitFeatureId)) {
      const auto fName = *dataset.getCategoricalFeatureName(bestSplitValue);
      ret->setBestSplitValueDescription(fName);
    }
    //
    // Prepare for the recursive step ------------------------------------------
    // Build the validFeaturesDownstream if isAffine (see line 1646 python code)
    const indexes_t validFeaturesDownstream =
        !isAffine ? indexes_t(validFeatures.begin(), validFeatures.end())
                  : [&validFeatures, &bestSplitFeatureId]() {
                      indexes_t ret;
                      for (const auto &f : validFeatures) {
                        if (f != bestSplitFeatureId) {
                          ret.emplace_back(f);
                        }
                      }
                      return ret;
                    }();
    // Get the left node
    Node *leftNode = fitRecursively(
        dataset, bestSplitLeftFeatureId, validFeaturesDownstream,
        currHeight + 1, attacker, costsLeft, bestPredLeft, maxDepth, minPerNode,
        isAffine, impurityType, constraintsLeft, numThreads, useICML2019);
    // Get the right node
    Node *rightNode = fitRecursively(
        dataset, bestSplitRightFeatureId, validFeaturesDownstream,
        currHeight + 1, attacker, costsRight, bestPredRight, maxDepth,
        minPerNode, isAffine, impurityType, constraintsRight, numThreads,
        useICML2019);
    // Make sure that we get a leaf or an internal node (leave left and right as nullptr)
    if (leftNode && rightNode) {
      ret->setLeft(leftNode);
      ret->setRight(rightNode);
    }
  }
  // Set node number of constraints
  ret->setNumberConstraints(constraints.size());
  //
  return ret;
}
