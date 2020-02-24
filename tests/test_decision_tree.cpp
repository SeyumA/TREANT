//
// Created by dg on 05/11/19.
//
#include "DecisionTree.h"
#include "Dataset.h"
#include <iostream>

int main() {
  std::cout << "Start of the program" << std::endl;

  Dataset dataset("../tests/small_db.txt");
  std::cout << dataset << std::endl;
  DecisionTree dt(4);
  dt.fit(dataset, 40, Impurity::SSE);

  std::cout << "Decision tree ----------------" << std::endl;
  std::cout << dt << std::endl << "It's height is " << dt.getHeight() << '\n';

  std::cout << "End of program" << std::endl;

  return 0;
}