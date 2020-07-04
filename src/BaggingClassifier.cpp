//
// Created by dg on 04/07/20.
//

#include "BaggingClassifier.h"
#include "Attacker.h"
#include "Dataset.h"

#include <cassert>
#include <iostream>
#include <random>
#include <thread>

void trainTrees(std::vector<DecisionTree> &trees,
                const indexes_t &treesToBeTrained,
                const std::vector<indexes_t> &randomRowIndexes,
                const Dataset &dataset, const Attacker &attacker,
                const bool &useICML2019, const unsigned &maxDepth,
                const unsigned &minPerNode, const bool &isAffine) {
  const unsigned innerThreads = 1;
  const Impurity impurity = Impurity::SSE;
  for (const auto &i : treesToBeTrained) {
    trees[i].fit(dataset, attacker, innerThreads, useICML2019, maxDepth,
                 minPerNode, isAffine, randomRowIndexes[i], impurity);
  }
}

BaggingClassifier::BaggingClassifier() : estimators_(1), jobs_(1) {}

void BaggingClassifier::fit(const Dataset &dataset, const Attacker &attacker,
                            const bool &useICML2019, const unsigned &maxDepth,
                            const unsigned &minPerNode, const bool &isAffine) {

  std::vector<DecisionTree>(estimators_).swap(trees_);
  const unsigned usedJobs = estimateOptimalJobs();

  // Assign tree indexes to jobs
  const auto treesPerJob = distributeTreeIdsOnJobs(usedJobs);

  // Vector of random indexes
  const std::vector<indexes_t> randomIndexesPerTree =
      generateRandomIndexesPerTree(dataset.rows_);

  std::vector<std::thread> tasks;
  for (unsigned int j = 0; j < usedJobs; j++) {
    tasks.push_back(std::thread(trainTrees, std::ref(trees_), treesPerJob[j],
                                randomIndexesPerTree, dataset, attacker,
                                useICML2019, maxDepth, minPerNode, isAffine));
  }
  for (auto &t : tasks) {
    t.join();
  }
}

void BaggingClassifier::setEstimators(const unsigned estimators) {
  assert(estimators > 0);
  estimators_ = estimators;
}

void BaggingClassifier::setJobs(const unsigned jobs) {
  assert(jobs > 0);
  jobs_ = jobs;
}

void BaggingClassifier::setMaxFeatures(const double maxFeatures) {
  assert(maxFeatures > 0.0);
  assert(maxFeatures <= 1.0);
  maxFeatures_ = maxFeatures;
}

unsigned BaggingClassifier::estimateOptimalJobs() const {
  if (jobs_ > estimators_) {
    std::cout << "WARNING: jobs > estimators; " << estimators_
              << " will be used\n";
    return estimators_;
  } else {
    return jobs_;
  }
}

std::vector<indexes_t>
BaggingClassifier::distributeTreeIdsOnJobs(unsigned jobs) const {
  std::vector<indexes_t> ret(jobs);
  for (unsigned int jobId = 0; jobId < jobs; jobId++) {
    for (unsigned int j = 0; j < estimators_; j++) {
      if (j % jobs == jobId) {
        ret[jobId].push_back(j);
      }
    }
  }
  return ret;
}

std::vector<indexes_t> BaggingClassifier::generateRandomIndexesPerTree(
    const unsigned &totalRows) const {

  std::vector<indexes_t> ret(estimators_);
  const unsigned indexesSize = static_cast<unsigned>(totalRows * maxFeatures_);
  std::default_random_engine generator;
  std::uniform_int_distribution<unsigned int> distribution(0, totalRows - 1);

  for (auto &treeIndexesSet : ret) {
    std::set<unsigned> indexesSet;
    unsigned counter = 0;
    while (counter < indexesSize) {
      const auto candidate = distribution(generator);
      const auto success = indexesSet.insert(candidate);
      if (success.second) {
        counter++;
      }
    }
    treeIndexesSet = indexes_t(indexesSet.begin(), indexesSet.end());
  }

  return ret;
}