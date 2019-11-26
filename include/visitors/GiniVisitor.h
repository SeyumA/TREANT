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

  [[nodiscard]] std::pair<INode *, partitions_t>
  getBestSplitterWithPartitions() const override;

  void prepareToVisit(index_t currentFeatureIndex) override;

private:
  index_t featureIndex_;
  const std::vector<index_t> &validIndexes_;
  const std::vector<label_t> &labels_;

  INode *bestSplitter_ = nullptr;
  double impurity_ = 0.0;
  std::vector<std::vector<index_t>> bestPartitions_;

  // Flag used to enforce the use of prepareToVisit function before visit.
  bool isPreparedToVisit_ = false;

  // Functions
private:
  double calculateImpurity(const std::vector<std::vector<index_t>> &partitions);
};

#endif // TREEANT_GINIVISITOR_H
