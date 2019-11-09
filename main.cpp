#include <iostream>
#include <iterator>
#include <regex>

#include "DecisionTree.h"
#include "Logger.h"
#include "OptParser.h"

struct Collection {

  typedef std::variant<int, double> feature_t;

  feature_t &operator[](int i) { return data_[validIndexes_[i]]; }

  const feature_t &operator[](int i) const { return data_[validIndexes_[i]]; }

  std::size_t size() { return validIndexes_.size(); }

  std::vector<feature_t> data_;
  std::vector<int> validIndexes_;
};

int main() {
  std::cout << "Start of the program" << std::endl;

  Collection c;
  c.data_ = {1, 2, 3, 4, 5, 6};
  c.validIndexes_ = {0, 2, 4};

  std::cout << "c[2] = " << std::get<int>(c[1]) << std::endl;
  std::cout << "c[1] = " << std::get<int>(c[1]) << std::endl;
  std::cout << "End of program" << std::endl;
  return 0;
}
