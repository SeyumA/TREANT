//
// Created by dg on 08/11/19.
//

#ifndef TREEANT_DATASET_H
#define TREEANT_DATASET_H

#include <cstdint>
#include <iosfwd>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

#include "types.h"

/**
 * The FeatureTypes enum must be consistent with feature_t
 */

class Dataset final {

public:
  explicit Dataset(const std::string &featureFilePath);

  // These functions are needed for node
  [[nodiscard]] const std::vector<label_t> &getLabels() const;

  [[nodiscard]] const std::vector<std::vector<feature_t>> &
  getFeatureColumns() const;

  [[nodiscard]] const std::vector<feature_t> &getFeatureColumn(index_t i) const;

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

  [[nodiscard]] bool empty() const { return labelVector_.empty(); }

  [[nodiscard]] std::size_t size() const { return labelVector_.size(); }

  [[nodiscard]] prediction_t getDefaultPrediction() const;

  friend std::ostream &operator<<(std::ostream &os, const Dataset &ds);

private:
  std::vector<std::string> featureNames_;
  std::vector<bool> featureIsNumeric_;

  std::unordered_map<std::string, feature_t> categoricalToDouble_;
  std::unordered_map<feature_t, std::string> categoricalToDoubleReversed_;

  std::vector<std::vector<feature_t>> featureColumns_;
  std::vector<label_t> labelVector_;
};

std::ostream &operator<<(std::ostream &os, const Dataset &ds);

#endif // TREEANT_DATASET_H
