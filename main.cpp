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

#include "nodes/Leaf.h"
#include "nodes/BooleanNode.h"
#include "nodes/BinIntNode.h"
#include "nodes/BinDoubleNode.h"

#include "visitors/GiniVisitor.h"

#include <cassert>


int main() {
  std::cout << "Start of the program" << std::endl;

  Dataset dataset("../tests/small_db.txt");
  std::cout << dataset << std::endl;
  DecisionTree dt(dataset, 3, DecisionTree::VisitorConstructorTypes::GINI);

  std::cout << "Decision tree ----------------" << std::endl;
  std::cout << dt << std::endl << "It's height is " << dt.getHeight() << '\n';

  std::cout << "End of program" << std::endl;
  return 0;
}
