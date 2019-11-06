//
// Created by dg on 05/11/19.
//

#include <cassert>

#include "DecisionTree.h"
//#include "nodes/BinIntNode.h"
//#include "nodes/Leaf.h"

int main() {
  dataset_t dataset = {
      {{false, 22}, 0},
      {{true, 55}, 1}
  };

  DecisionTree dt(dataset, 1);

//  record_t r = {23, "pippo", "pluto"};
//  auto labelPrediction = dt.predict(r);
//  assert(labelPrediction == 1);

  return 0;
}