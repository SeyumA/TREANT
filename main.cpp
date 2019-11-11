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

struct IColumn {
  [[nodiscard]] virtual std::vector<std::vector<std::size_t>>
  classify() const = 0;
};

struct Int32Column : public IColumn {
  [[nodiscard]] std::vector<std::vector<std::size_t>>
  classify() const override {
    auto partitions = std::vector<std::vector<std::size_t>>();
    std::cout << "Calling Int32Column classify()\n";
    return partitions;
  }

  void print() {

  }
};

struct BoolColumn : public IColumn {
  [[nodiscard]] std::vector<std::vector<std::size_t>>
  classify() const override {
    auto partitions = std::vector<std::vector<std::size_t>>();
    std::cout << "Calling BoolColumn classify()\n";
    return partitions;
  }
};

struct MyDataset {

  MyDataset() {
    //    IColumn firstCol;
    columns.push_back(std::make_shared<Int32Column>(Int32Column()));
    columns.push_back(std::make_shared<BoolColumn>(BoolColumn()));


    std::shared_ptr<IColumn> firstCol (new Int32Column);
    columns.push_back(std::move(firstCol));

    std::shared_ptr<IColumn> secondCol (new BoolColumn);
    columns.push_back(std::move(secondCol));
//    BoolColumn secondCol;
//    columns.push_back(std::make_shared<IColumn>(secondCol));
  }

  void classifyColumns() {
    for (auto col : columns) {
      auto partitions = col->classify();
    }
  }

  std::vector<std::shared_ptr<IColumn>> columns;
};

int main() {
  std::cout << "Start of the program" << std::endl;

  MyDataset myDataset;
  myDataset.classifyColumns();

  Collection c;
  c.data_ = {1, 2, 3, 4, 5, 6};
  c.validIndexes_ = {0, 2, 4};

  std::cout << "c[2] = " << std::get<int>(c[1]) << std::endl;
  std::cout << "c[1] = " << std::get<int>(c[1]) << std::endl;
  std::cout << "End of program" << std::endl;
  return 0;
}
