//
// Created by dg on 08/11/19.
//

#ifndef TREEANT_DATASET_H
#define TREEANT_DATASET_H

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

#include "types.h"

/**
 * The FeatureTypes enum must be consistent with feature_t
 */

class Dataset final {

public:
  explicit Dataset(const std::string &featureFilePath);

  // These functions are needed for node
  [[nodiscard]] const std::vector<label_t> &getLabels() const;

  [[nodiscard]] const std::vector<feature_vector_t> &getFeatureColumns() const;

  [[nodiscard]] const feature_vector_t &getFeatureColumn(index_t i) const;

  [[nodiscard]] std::pair<label_t, frequency_t>
  getMostFrequentLabel(const std::vector<index_t> &validIndexes) const;

  [[nodiscard]] bool empty() const { return labelVector_.empty(); }

  [[nodiscard]] std::size_t size() const { return labelVector_.size(); }

  friend std::ostream &operator<<(std::ostream &os, const Dataset &dt);

private:
  std::vector<std::string> featureNames_;
  std::vector<feature_vector_t> featureColumns_;
  std::vector<label_t> labelVector_;
};

std::ostream &operator<<(std::ostream &os, const Dataset &dt);

#endif // TREEANT_DATASET_H
