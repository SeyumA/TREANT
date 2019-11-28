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

  std::cout << utils::format("hello '{}' pluto '{}'", 3.4555, 6) << std::endl;

  // Dataset myDataset("", "");
  std::cout << "End of program" << std::endl;
  return 0;
}
