//
// Created by dg on 05/11/19.
//

#include <cassert>

#include "DecisionTree.h"
#include "nodes/BinIntNode.h"
#include "nodes/Leaf.h"

int main() {
  DecisionTree dt =
      DecisionTree(2, new BinIntNode(0, 22, new Leaf(0), new Leaf(1)));

  record_t r = {23, "pippo", "pluto"};
  auto labelPrediction = dt.predict(r);
  assert(labelPrediction == 1);

  return 0;
}