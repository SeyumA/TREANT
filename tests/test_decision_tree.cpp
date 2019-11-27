//
// Created by dg on 05/11/19.
//

#include <cassert>

#include "DecisionTree.h"

int main() {
  // TODO: pass the right paths to the files
  Dataset dataset("", "");

  DecisionTree dt(dataset, 1, DecisionTree::VisitorConstructorTypes::GINI);

  return 0;
}