//
// Created by dg on 18/11/19.
//

#ifndef TREEANT_IMPURITYVISITOR_H
#define TREEANT_IMPURITYVISITOR_H

#include "IFeatureVectorVisitor.h"

class BoolFeatureVector;
class IntFeatureVector;
class DoubleFeatureVector;

class ImpurityVisitor : public IFeatureVectorVisitor {
public:
  ~ImpurityVisitor();

  void visit(const BoolFeatureVector *featureVector,
             const indexes_t &validIndexes) const override;

  void visit(const IntFeatureVector *featureVector,
             const indexes_t &validIndexes) const override;

  void visit(const DoubleFeatureVector *featureVector,
             const indexes_t &validIndexes) const override;
};

#endif // TREEANT_IMPURITYVISITOR_H
