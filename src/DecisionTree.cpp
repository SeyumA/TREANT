//
// Created by dg on 01/11/19.
//

#include "DecisionTree.h"

DecisionTree::DecisionTree(int depth, INode *root) : depth(depth), root(root) {}

DecisionTree::DecisionTree(dataset_t dataset) : depth(0), root(nullptr) {
  // TODO: must be implemented
  for (const auto& record : dataset) {
    record[0];
  }
  // Find the best split
}

DecisionTree::~DecisionTree() {
  delete root;
  root = nullptr;
}


int DecisionTree::predict(const record_t& r) const {
  return root->predict(r);
}
