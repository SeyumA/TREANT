//
// Created by dg on 18/11/19.
//

#ifndef TREEANT_GINIVISITOR_H
#define TREEANT_GINIVISITOR_H

#include "IFeatureVectorVisitor.h"

class INode;

class GiniVisitor : public IFeatureVectorVisitor {
public:
  explicit GiniVisitor(const std::vector<index_t> &validIndexes,
                       const std::vector<label_t> &labels);

  void operator()(const bool_vector_t &) override;

  void operator()(const int_vector_t &) override;

  void operator()(const double_vector_t &doubleVector) override;

  void visitFeatureVectors(
      const std::vector<feature_vector_t> &featureVectors) override;

  [[nodiscard]] IFeatureVectorVisitor *clone() const override;

  [[nodiscard]] std::pair<INode *, partitions_t>
  getBestSplitterWithPartitions() const override;

private:

  index_t featureIndex_;
  const std::vector<index_t> &validIndexes_;
  const std::vector<label_t> &labels_;
  INode *bestSplitter_;

  double impurity_;
  partitions_t bestPartitions_;

  // Private functions
  double calculateImpurity(const std::vector<std::vector<index_t>> &partitions);

  // Returns true in the case of a successful assignment, otherwise false
  // (the node must be deleted)
  void tryAssignNode(INode* node, partitions_t& partitions);
};

#endif // TREEANT_GINIVISITOR_H
