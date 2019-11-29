#include <iostream>
#include <iterator>
#include <regex>
#include <variant>

#include "Dataset.h"
#include "DecisionTree.h"
#include "Logger.h"
#include "OptParser.h"
#include "features/BoolFeatureVector.h"
#include "features/DoubleFeatureVector.h"
#include "features/IntFeatureVector.h"
#include "splitters/DummySplitter.h"
#include "visitors/GiniVisitor.h"

#include <cassert>

int main() {
  std::cout << "Start of the program" << std::endl;

  Dataset dataset("../tests/small_db.txt");
  std::cout << dataset << std::endl;
  DecisionTree dt(dataset, 1, DecisionTree::VisitorConstructorTypes::GINI);

  std::cout << "End of program" << std::endl;
  return 0;
}
