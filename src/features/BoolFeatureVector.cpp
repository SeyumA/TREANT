//
// Created by dg on 11/11/19.
//

#include "features/BoolFeatureVector.h"
#include <nodes/BooleanNode.h>

BoolFeatureVector::BoolFeatureVector(std::vector<bool> &&features)
    : features_(std::move(features)) {}

[[nodiscard]] std::tuple<std::vector<partition_t>, INode *, double>
BoolFeatureVector::split(const std::vector<std::size_t> &validIndexes) const {

  std::vector<partition_t> res(2);

  for (const auto &i : validIndexes) {
    if (!features_[i]) {
      res[0].push_back(i);
    } else {
      res[1].push_back(i);
    }
  }

  // TODO: calculate impurity with the splitter
  double impurity = 0.0;

  return std::make_tuple(res, new BooleanNode(0), impurity);
}

std::size_t BoolFeatureVector::size() const { return features_.size(); }
