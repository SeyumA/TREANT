//
// Created by dg on 08/11/19.
//

#ifndef TREEANT_DATASET_H
#define TREEANT_DATASET_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "types.h"
#include <features/IFeatureVector.h>

/**
 * The FeatureTypes enum must be consistent with feature_t
 */
enum FeatureTypes { BOOL = 0x0, INT, DOUBLE };

class Dataset final {

public:
  explicit Dataset(const std::string &featureFilePath,
                   const std::string &labelFilePath);

  [[nodiscard]] std::size_t size() const;
  [[nodiscard]] const std::vector<std::int32_t> &getLabels() const;
  [[nodiscard]] const std::vector<std::shared_ptr<IFeatureVector>> &
  getFeatureColumns() const;

private:
  std::vector<FeatureTypes> headers_;
  std::vector<std::shared_ptr<IFeatureVector>> featureColumns_;
  std::vector<std::int32_t> labelVector_;
};

class DataSubset final {
public:
  explicit DataSubset(const Dataset &parent);
  explicit DataSubset(const Dataset &parent,
                      std::vector<std::size_t> &&validIndexes);

  // TODO: implement accessors []

  [[nodiscard]] std::size_t getSize() const;
  [[nodiscard]] std::pair<label_t, frequency_t> getMostFrequentLabel() const;

private:
  const Dataset &parent_;
  indexes_t validIndexes_;
};

#endif // TREEANT_DATASET_H
