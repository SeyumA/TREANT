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
#include "visitors/ImpurityVisitor.h"

// --------------------------------------------------------------------
// from https://www.bfilipek.com/2019/06/fsm-variant-game.html

struct IntColumn {
  IntColumn(std::vector<std::int32_t> &&data) : data_(data) {}
  std::vector<std::int32_t> data_;
};

struct BoolColumn {
  BoolColumn(std::vector<bool> &&data) : data_(data) {}
  std::vector<bool> data_;
};

using ColumnState =
    std::variant<BoolFeatureVector, IntFeatureVector, DoubleFeatureVector>;

template <class... Ts> struct overload : Ts... { using Ts::operator()...; };
template <class... Ts> overload(Ts...)->overload<Ts...>;

template <typename Variant, typename... Matchers>
auto match(Variant &&variant, Matchers &&... matchers) {
  return std::visit(overload{std::forward<Matchers>(matchers)...},
                    std::forward<Variant>(variant));
}

int main() {
  std::cout << "Start of the program" << std::endl;

  // Dataset myDataset("", "");
  std::cout << "End of program" << std::endl;
  return 0;
}
