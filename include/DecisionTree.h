//
// Created by dg on 01/11/19.
//

#ifndef TREEANT_DECISIONTREE_H
#define TREEANT_DECISIONTREE_H

#include <functional>
#include <iostream>
#include <utility>
#include <variant>
#include <vector>

#include "nodes/INode.h"

typedef std::vector<record_t> dataset_t;

class DecisionTree final {

public:
  // Constructors
  explicit DecisionTree(int depth, INode* root);
  DecisionTree(dataset_t dataset);
  // Destructor
  ~DecisionTree();

  // Functions
  [[nodiscard]] int predict(const record_t& r) const;

private:
  int depth;
  INode* root = nullptr;
};

#endif // TREEANT_DECISIONTREE_H
