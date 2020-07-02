//
// Created by dg on 08/11/19.
//

#ifndef TREEANT_DATASET_H
#define TREEANT_DATASET_H

#include <cstdint>
#include <iosfwd>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "types.h"

/**
 * Memory management of X and y is left to the caller
 */

class Dataset final {

public:
  // Used by python and by C++ but previously the data must be extracted
  // from the dataset file using the static functions provided.
  explicit Dataset(const double *X, const unsigned rows, const unsigned cols,
                   const double *y, const std::string &isNumerical,
                   const std::string &notNumericalEntries,
                   const std::string &columnNames);

  // Returns rows, columnNames having size = columns
  static std::pair<unsigned, std::vector<std::string>>
  getDatasetInfoFromFile(const std::string &datasetFilePath);

  // Fills vectors X and y and returns isNumerical and notNumericalEntries
  static std::pair<std::vector<std::string>, std::vector<std::string>>
  fillXandYfromFile(double *X, const unsigned rows, const unsigned cols,
                    double *y, const std::string &datasetFilePath);

  [[nodiscard]] bool empty() const;
  [[nodiscard]] std::size_t size() const;

  // These functions are needed for node
  [[nodiscard]] bool isFeatureNumerical(index_t j) const;

  [[nodiscard]] std::string getFeatureName(index_t i) const;

  [[nodiscard]] index_t getFeatureIndex(const std::string &featureName) const;

  [[nodiscard]] std::optional<feature_t>
  getCategoricalFeatureValue(const std::string &featureName) const;

  [[nodiscard]] std::optional<std::string>
  getCategoricalFeatureName(const feature_t &featureValue) const;

  [[nodiscard]] record_t getRecord(index_t i) const;

  //  [[nodiscard]] std::pair<label_t, frequency_t>
  //  getMostFrequentLabel(const std::vector<index_t> &validIndexes) const;

  [[nodiscard]] prediction_t getDefaultPrediction() const;

  friend std::ostream &operator<<(std::ostream &os, const Dataset &ds);

  // Read-only accessors -------------------------------------------------------
  // Accessor to X that is stored column-wise
  feature_t operator()(std::size_t i, std::size_t j) const {
    if (i < rows_ && j < cols_) {
      return X_[j * rows_ + i];
    }
    throw std::runtime_error("Array index out of bound, exiting");
  }
  // Accessor to y
  label_t operator()(std::size_t i) const {
    if (i < rows_) {
      return y_[i];
    }
    throw std::runtime_error("Array index out of bound, exiting");
  }

public:
  const std::size_t rows_;
  const std::size_t cols_;
  const feature_t *X_;
  const label_t *y_;

private:
  std::vector<std::string> featureNames_;
  std::vector<bool> featureIsNumeric_;

  std::unordered_map<std::string, feature_t> categoricalToDouble_;
  std::unordered_map<feature_t, std::string> categoricalToDoubleReversed_;
};

std::ostream &operator<<(std::ostream &os, const Dataset &ds);

#endif // TREEANT_DATASET_H
