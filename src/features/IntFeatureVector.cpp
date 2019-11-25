//
// Created by dg on 11/11/19.
//

#include "features/IntFeatureVector.h"
#include "visitors/IFeatureVectorVisitor.h"

IntFeatureVector::IntFeatureVector(std::vector<std::int32_t> &&features)
    : features_(std::move(features)) {}

[[nodiscard]] std::tuple<std::vector<partition_t>, INode *, double>
IntFeatureVector::split(const std::vector<std::size_t> &) const {

  std::vector<partition_t> res;

  // TODO: implement this

  return std::make_tuple(res, nullptr, 0.0);
}

std::size_t IntFeatureVector::size() const { return features_.size(); }

void IntFeatureVector::accept(IFeatureVectorVisitor *visitor,
                                 const indexes_t &validIndexes) const {
  visitor->visit(this, validIndexes);
}
