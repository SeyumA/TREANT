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

struct IFVector;
struct IntFVector;
struct BoolFVector;

struct IBuildingVisitor {
  virtual ~IBuildingVisitor() = default;

  virtual void visit(const IntFVector *featureVector,
                     const indexes_t &validIndexes) = 0;

  virtual void visit(const BoolFVector *featureVector,
                     const indexes_t &validIndexes) = 0;
};

struct DummyBuildingVisitor : public IBuildingVisitor {
  friend class IntFVector;
  friend class BoolFVector;

  //  void visit(const IFVector *featureVector,
  //             const indexes_t& validIndexes) {
  //    std::cout << "visit overloading IFVector\n";
  //  }

  void visit(const IntFVector *intFVector, const indexes_t &validIndexes) {
    std::cout << "visit overloading IntFVector\n";
  }

  void visit(const BoolFVector *boolFVector, const indexes_t &validIndexes) {
    std::cout << "visit overloading BoolFVector\n";
  }
};

struct IFVector {
  virtual ~IFVector() = default;
  virtual void accept(IBuildingVisitor *visitor,
                      const indexes_t &validIndexes) = 0;
};

struct BoolFVector : public IFVector {
  void accept(IBuildingVisitor *visitor, const indexes_t &validIndexes) {
    visitor->visit(this, validIndexes);
  }
};

struct IntFVector : public IFVector {
  void accept(IBuildingVisitor *visitor, const indexes_t &validIndexes) {
    visitor->visit(this, validIndexes);
  }
};

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

int main(int argc, char *argv[]) {
  std::cout << "Start of the program" << std::endl;

  // Dataset myDataset("", "");

  {
    IBuildingVisitor *visitor = new DummyBuildingVisitor{};
    IFVector *boolFVector = new BoolFVector{};
    IFVector *intFVector = new IntFVector{};
    indexes_t indexes;

    boolFVector->accept(visitor, indexes);

    intFVector->accept(visitor, indexes);

    delete visitor;
    delete boolFVector;
    delete intFVector;
  }

  { ImpurityVisitor vis; }

  {
    std::vector<std::int32_t> vInt = {1, 2, 3};
    if (argc == 1) {
      vInt.push_back(4);
      vInt.push_back(5);
      vInt.push_back(6);
    }
    //    std::vector<bool> ;
    std::vector<ColumnState> myArray = {
        IntFeatureVector(std::move(vInt)),
        BoolFeatureVector{{true, false, true, false}},
        DoubleFeatureVector{{2.5, 3.5, 4.0}}};
    //    std::cout << "vBool[0] = " << vBool[0] << std::endl;

    const std::vector<std::size_t> validIndexes = {0,2};
    for (auto& col : myArray) {
      const auto [impurity, node, array] = match(
          col,
          [&validIndexes](DoubleFeatureVector &vec) {
            // implement this
            double sum = 0;
            for (const auto i : validIndexes) {
              sum += vec[i];
            }
            std::cout << "Processing IntFeatureVector\n";
            double mean = sum / vec.size();
            partitions_t p;
            return std::make_tuple(mean, nullptr, p);
          },

          [&validIndexes](IntFeatureVector &vec) {
            // implement this
            int sum = 0;
            for (const auto i : validIndexes) {
              sum += vec[i];
            }
            std::cout << "Processing IntFeatureVector\n";
            double mean = static_cast<double>(sum) / vec.size();
            partitions_t p;
            return std::make_tuple(mean, nullptr, p);
          },

          [&validIndexes](BoolFeatureVector &vec) {
            // implement this
            std::cout << "Processing BoolColumn\n";
            int numFalse = 0;
            for (const auto i : validIndexes) {
              if (vec[i])
                numFalse++;
            }

            double mean = static_cast<double>(numFalse) / vec.size();
            partitions_t p;
            return std::make_tuple(mean, nullptr, p);
          });

      std::cout << "impurity = " << impurity << std::endl;
    } // end loop
  }  // end of test scope

  std::cout << "End of program" << std::endl;
  return 0;
}
