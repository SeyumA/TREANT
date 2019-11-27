//
// Created by dg on 18/11/19.
//

#ifndef TREEANT_GINIVISITOR_H
#define TREEANT_GINIVISITOR_H

#include "IFeatureVectorVisitor.h"
#include "nodes/BinDoubleNode.h"
#include "nodes/BinIntNode.h"
#include "nodes/BooleanNode.h"

class GiniVisitor : public IFeatureVectorVisitor {
public:
  explicit GiniVisitor(const std::vector<index_t> &validIndexes,
                       const std::vector<label_t> &labels);

  void operator()(const bool_vector_t &) override;

  void operator()(const int_vector_t &) override;

  void operator()(const double_vector_t &doubleVector) override;

  void visitFeatureVectors(
      const std::vector<feature_vector_t> &featureVectors) override;

  [[nodiscard]] IFeatureVectorVisitor* clone() const override;

  [[nodiscard]] std::pair<INode *, partitions_t>
  getBestSplitterWithPartitions() const override;

protected:
  // Leaved virtual in order to be able to change it in a subclass.
  virtual double calculateImpurity(const std::vector<std::vector<index_t>> &partitions);

private:
  index_t featureIndex_;
  const std::vector<index_t> &validIndexes_;
  const std::vector<label_t> &labels_;

  INode *bestSplitter_;
  double impurity_;
  std::vector<std::vector<index_t>> bestPartitions_;
};

#endif // TREEANT_GINIVISITOR_H
