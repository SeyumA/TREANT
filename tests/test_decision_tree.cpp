//
// Created by dg on 05/11/19.
//
#include <cassert>
#include <iostream>
#include "DecisionTree.h"

int main() {
  Dataset dataset("../tests/small_db.txt");
  std::cout << "The dataset is:" << std::endl << dataset << std::endl;
  std::size_t maxDepth = 4;
  DecisionTree dt(dataset, maxDepth, DecisionTree::VisitorConstructorTypes::GINI);
  std::cout << "The decision tree is:" << std::endl << dt << std::endl;
  const auto height = dt.getHeight();
  assert(height == 3);
  // Build a test record and try to predict it.
  // The arguments order must match the column feature order in the dataset.
  record_t record{false, 9.1, 3};
  const auto predictedLabel = dt.predict(record);
  assert(predictedLabel == 1);
  return 0;
}