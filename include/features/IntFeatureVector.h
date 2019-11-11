//
// Created by dg on 11/11/19.
//

#ifndef TREEANT_INTFEATUREVECTOR_H
#define TREEANT_INTFEATUREVECTOR_H

// STD library
#include <cstdint>
// Local headers
#include "IFeatureVector.h"

class IntFeatureVector : public IFeatureVector {

public:
  explicit IntFeatureVector(std::vector<std::int32_t> &&);

  [[nodiscard]] std::tuple<std::vector<partition_t>, INode *, double>
  split(const std::vector<std::size_t> &validIndexes) const override;
  [[nodiscard]] std::size_t size() const override;

private:
  std::vector<std::int32_t> features_;
};

#endif // TREEANT_INTFEATUREVECTOR_H
