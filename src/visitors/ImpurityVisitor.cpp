//
// Created by dg on 18/11/19.
//

#include "visitors/ImpurityVisitor.h"

ImpurityVisitor::~ImpurityVisitor() = default;

void ImpurityVisitor::visit(const BoolFeatureVector *featureVector,
                            const indexes_t &validIndexes) const {

  // TODO: implement this
  partitions_t p;
}

void ImpurityVisitor::visit(const IntFeatureVector *featureVector,
                            const indexes_t &validIndexes) const {

  // TODO: implement this
  partitions_t p;
}

void ImpurityVisitor::visit(const DoubleFeatureVector *featureVector,
                            const indexes_t &validIndexes) const {

  // TODO: implement this
  partitions_t p;
}
