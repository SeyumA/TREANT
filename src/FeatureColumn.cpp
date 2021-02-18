//
// Created by dg on 19/02/20.
//

#include "FeatureColumn.h"
#include <stdexcept>

void FeatureColumn::addElement(const std::string &token) {
  fp_feature_t toAdd = 0.0;
  try {
    toAdd = std::stod(token);
  } catch (std::invalid_argument &) {
    if (!fp_feature_vec_.empty()) {
      throw std::runtime_error(
          "Cannot add a categorical feature to a numerical feature column");
    }
    ct_feature_vec_.emplace_back(token);
    return;
  }
  if (!ct_feature_vec_.empty()) {
    throw std::runtime_error(
        "Cannot add a numerical feature to a categorical feature column");
  }
  fp_feature_vec_.emplace_back(toAdd);
}

bool FeatureColumn::isNumerical() const {
  if (empty()) {
    throw std::runtime_error("Cannot call isNumerical on an empty column");
  }
  return !fp_feature_vec_.empty();
}

bool FeatureColumn::empty() const {
  return fp_feature_vec_.empty() && ct_feature_vec_.empty();
}

std::size_t FeatureColumn::size() const {
  return empty() ? 0 : (fp_feature_vec_.size() + ct_feature_vec_.size());
}