//
// Created by dg on 04/07/20.
//

#ifndef TREEANT_BAGGINGCLASSIFIER_H
#define TREEANT_BAGGINGCLASSIFIER_H

#include "DecisionTree.h"
#include "types.h"
#include <vector>
#include <set>

class Dataset;
class Attacker;

class BaggingClassifier {

public:
  explicit BaggingClassifier();

  void fit(const Dataset &dataset, const Attacker &attacker,
           const bool &useICML2019, const unsigned &maxDepth,
           const unsigned &minPerNode, const bool &isAffine);

  void predict(const double *X, const unsigned &rows, const unsigned &cols,
               double *res, const bool &isRowsWise) const;

  void setEstimators(const unsigned &estimators);
  void setJobs(const unsigned &jobs);
  void setMaxFeatures(const double &maxFeatures);

private:
  unsigned estimators_ = 1;
  unsigned jobs_ = 1;
  double maxFeatures_ = 0.5;
  std::vector<DecisionTree> trees_;
  std::set<label_t> classes_;

  [[nodiscard]] unsigned estimateOptimalJobs() const;
  [[nodiscard]] std::vector<indexes_t>
  distributeTreeIdsOnJobs(unsigned jobs) const;

  [[nodiscard]] std::vector<indexes_t>
  generateRandomIndexesPerTree(const unsigned &totalRows) const;
};

#endif // TREEANT_BAGGINGCLASSIFIER_H
