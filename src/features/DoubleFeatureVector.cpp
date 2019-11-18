//
// Created by dg on 11/11/19.
//

#include "features/DoubleFeatureVector.h"
#include "visitors/IFeatureVectorVisitor.h"

DoubleFeatureVector::DoubleFeatureVector(std::vector<double> &&features)
    : features_(std::move(features)) {}

[[nodiscard]] std::tuple<std::vector<partition_t>, INode *, double>
DoubleFeatureVector::split(const std::vector<std::size_t> &validIndexes) const {

  std::vector<partition_t> res;

  // TODO: implement this

  return std::make_tuple(res, nullptr, 0.0);
}

std::size_t DoubleFeatureVector::size() const { return features_.size(); }

void DoubleFeatureVector::accept(IFeatureVectorVisitor *visitor,
                                 const indexes_t &validIndexes) const {
  visitor->visit(this, validIndexes);
}
