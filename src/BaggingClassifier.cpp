//
// Created by dg on 04/07/20.
//

#include "BaggingClassifier.h"
#include "Attacker.h"
#include "Dataset.h"
#include "utils.h"

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

  // Determine possible classes and assign to classes_ in analogy with python
  // BaggingClassifier
  std::set<label_t> classesSet;
  for (std::size_t i = 0; i < dataset.rows_; i++) {
    classesSet.insert(dataset(i));
  }

  if (classesSet.size() != 2) {
    throw std::runtime_error(utils::format(
        "This BaggingClassifier is designed only for binary classification, "
        "detected {} types of labels",
        classesSet.size()));
  }
  // Update classes
  classesSet.swap(classes_);

  std::vector<DecisionTree>(estimators_).swap(trees_);
  const unsigned usedJobs = estimateOptimalJobs();

  // Assign tree indexes to jobs
  const auto treesPerJob = distributeTreeIdsOnJobs(usedJobs);

  // Vector of random indexes
  const std::vector<indexes_t> randomIndexesPerTree =
      generateRandomIndexesPerTree(dataset.rows_);

  std::vector<std::thread> tasks;
  for (unsigned int j = 0; j < usedJobs; j++) {
    tasks.emplace_back(std::thread(
        trainTrees, std::ref(trees_), treesPerJob[j], randomIndexesPerTree,
        dataset, attacker, useICML2019, maxDepth, minPerNode, isAffine));
  }
  for (auto &t : tasks) {
    t.join();
  }
}

void BaggingClassifier::predict(const double *X, const unsigned &rows,
                                const unsigned &cols, double *res,
                                const bool &isRowsWise) const {
  assert(!classes_.empty());
  assert(!trees_.empty());
  // score is false because real prediction are needed: 0.0 or 1.0
  const bool score = false;

  double *allPredictions = (double *)malloc(sizeof(double) * rows * trees_.size());
  unsigned offset = 0;
  for (const auto &tree : trees_) {
    tree.predict(X, rows, cols, allPredictions + offset, isRowsWise, score);
    offset += rows;
  }

  std::unordered_map<label_t, unsigned> votes;
  for (const auto &c : classes_) {
    votes[c] = 0;
  }

  for (std::size_t i = 0; i < rows; i++) {
    // Update the votes map with the results of the current record
    for (std::size_t t = 0; t < trees_.size(); t++) {
      const double currPrediction = allPredictions[t * rows + i];
      assert(votes.find(currPrediction) != votes.end());
      votes[currPrediction] += 1;
    }
    // Calculate the most voted
    label_t mostVotedClass = votes.begin()->first;
    unsigned mostVotedVotes = votes.begin()->second;
    for (const auto &[c, v] : votes) {
      if (v > mostVotedVotes) {
        mostVotedClass = c;
        mostVotedVotes = v;
      }
    }
    // Update the result vector
    res[i] = mostVotedClass;
    // Reset the map
    for (const auto &c : classes_) {
      votes[c] = 0;
    }
  }
  free((void *)allPredictions);
}

void BaggingClassifier::setEstimators(const unsigned &estimators) {
  assert(estimators > 0);
  estimators_ = estimators;
}

void BaggingClassifier::setJobs(const unsigned &jobs) {
  assert(jobs > 0);
  jobs_ = jobs;
}

void BaggingClassifier::setMaxFeatures(const double &maxFeatures) {
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