//
// Created by dg on 08/11/19.
//

#ifndef TREEANT_DATASET_H
#define TREEANT_DATASET_H

#include <cstdint>
#include <string>
#include <utility>
#include <variant>
#include <vector>

typedef std::int32_t label_t;
typedef std::variant<bool, std::int32_t, double> feature_t;
typedef std::vector<feature_t> record_t;
typedef std::vector<std::pair<record_t, label_t>> dataset_t;

/**
 * The FeatureTypes enum must be consistent with feature_t
 */
enum FeatureTypes { BOOL = 0x0, INT32, DOUBLE };

class ColumnFeature {

  typedef std::vector<std::size_t> partition_t;

public:

  explicit ColumnFeature(std::vector<feature_t> data)
      : data_(std::move(data)) {}

  [[nodiscard]] const std::vector<feature_t> &getData() const { return data_; }

  std::vector<partition_t> classify() {
    std::vector<partition_t> res;
//    using T = std::decay_t<decltype(data_[0])>;
//    if constexpr (std::is_same_v<T, bool>) {
//      std::cout << "int with value " << arg << '\n';
//    }
//
//    for (const auto& feature : data_){
//      std::visit([](auto&& arg) {
//        using T = std::decay_t<decltype(arg)>;
//        if constexpr (std::is_same_v<T, bool>) {
//          std::cout << "int with value " << arg << '\n';
//        }
//
//        else if constexpr (std::is_same_v<T, long>)
//          std::cout << "long with value " << arg << '\n';
//        else if constexpr (std::is_same_v<T, double>)
//          std::cout << "double with value " << arg << '\n';
//        else if constexpr (std::is_same_v<T, std::string>)
//          std::cout << "std::string with value " << std::quoted(arg) << '\n';
//        else
//            static_assert(always_false<T>::value, "non-exhaustive visitor!");
//      }, feature);
//    }

    return res;
  }

private:
  std::vector<feature_t> data_;
  std::vector<std::size_t> invalidIndexes_;
};


class ColumnSplitter {

private:

  typedef std::vector<std::size_t> partition_t;
  std::vector<partition_t> partitions;

  struct Visitor {

    explicit Visitor(ColumnSplitter& parent) : parent_(parent) {
      parent_.partitions.clear();
    }

    void operator()(bool& _in) {
      //
      if (parent_.partitions.empty()) {
        parent_.partitions.resize(2);
      }
      //
      if (!_in) {
        parent_.partitions[0].push_back(currentIndex);
      } else {
        parent_.partitions[1].push_back(currentIndex);
      }
    }

    void operator()(std::int32_t & _in) {
      _in += _in;
    }

    void operator()(double& _in) {
      _in += _in;
    }

    std::size_t currentIndex;
    std::vector<std::int32_t> intCriteria;
    std::vector<double> doubleCriteria;

  private:
    ColumnSplitter& parent_;
  };

public:

  void classify(const ColumnFeature &column) {
    const auto &columnData = column.getData();
    std::vector<partition_t> res;
    Visitor visitor(*this);
    for (auto data : columnData) {

      std::visit(visitor, data);
    }
  }
};

class Dataset {

public:
  explicit Dataset(const std::string &featureFilePath,
                   const std::string &labelFilePath);
  explicit Dataset(std::vector<FeatureTypes>, dataset_t);

  [[nodiscard]] inline std::size_t size() const { return dataset_.size(); }

private:
  std::vector<FeatureTypes> headers_;
  dataset_t dataset_;
};

class DataSubset {
public:
  explicit DataSubset(const Dataset &parent)
      : parent_(parent),
        validIndexes_(std::vector<std::size_t>(parent.size())) {}

  explicit DataSubset(const Dataset &parent,
                      std::vector<std::size_t> validIndexes)
      : parent_(parent), validIndexes_(std::move(validIndexes)) {}

private:
  const Dataset &parent_;
  std::vector<std::size_t> validIndexes_;
};

#endif // TREEANT_DATASET_H
