//
// Created by dg on 11/11/19.
//

#ifndef TREEANT_BOOLFEATUREVECTOR_H
#define TREEANT_BOOLFEATUREVECTOR_H

#include "IFeatureVector.h"

class BoolFeatureVector : public IFeatureVector {

public:
  explicit BoolFeatureVector(std::vector<bool> &&);

  [[nodiscard]] std::tuple<std::vector<partition_t>, INode *, double>
  split(const std::vector<std::size_t> &validIndexes) const override;
  [[nodiscard]] std::size_t size() const override;

private:
  std::vector<bool> features_;
};

#endif // TREEANT_BOOLFEATUREVECTOR_H
