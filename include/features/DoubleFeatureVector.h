//
// Created by dg on 11/11/19.
//

#ifndef TREEANT_DOUBLEFEATUREVECTOR_H
#define TREEANT_DOUBLEFEATUREVECTOR_H

#include "IFeatureVector.h"

class DoubleFeatureVector : public IFeatureVector {

public:
  explicit DoubleFeatureVector(std::vector<double> &&);

  [[nodiscard]] std::tuple<std::vector<partition_t>, INode *, double>
  split(const std::vector<std::size_t> &validIndexes) const override;
  [[nodiscard]] std::size_t size() const override;

  void accept(IFeatureVectorVisitor *visitor,
              const indexes_t &validIndexes) const override;

private:
  std::vector<double> features_;
};

#endif // TREEANT_DOUBLEFEATUREVECTOR_H
