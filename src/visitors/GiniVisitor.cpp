//
// Created by dg on 18/11/19.
//

#include "visitors/GiniVisitor.h"
#include <map>

GiniVisitor::GiniVisitor(const std::vector<index_t> &validIndexes,
                                 const std::vector<label_t> &labels)
    : featureIndex_(0), validIndexes_(validIndexes), labels_(labels) {}

void GiniVisitor::prepareToVisit(index_t currentFeatureIndex) {
  featureIndex_ = currentFeatureIndex;
  isPreparedToVisit_ = true;
}

void GiniVisitor::operator()(const bool_vector_t &boolVector) {
  if (!isPreparedToVisit_) {
    throw std::runtime_error(
        "The GiniVisitor is not prepared to visit a bool_vector_t");
  }
  // This visitor uses a BooleanNode to split a bool_vector_t
  if (!bestSplitter_) {
    bestSplitter_ = new BooleanNode(featureIndex_);
    auto partitions = bestSplitter_->split(validIndexes_, boolVector);
    impurity_ = calculateImpurity(partitions);
    std::swap(partitions, bestPartitions_);
  } else {
    INode *candidate = new BooleanNode(featureIndex_);
    auto partitions = candidate->split(validIndexes_, boolVector);
    const double candidateImpurity = calculateImpurity(partitions);
    if (candidateImpurity < impurity_) {
      delete bestSplitter_;
      bestSplitter_ = candidate;
      impurity_ = candidateImpurity;
      std::swap(partitions, bestPartitions_);
    } else {
      delete candidate;
    }
  }
  isPreparedToVisit_ = false;
}

void GiniVisitor::operator()(const int_vector_t &intVector) {
  if (!isPreparedToVisit_) {
    throw std::runtime_error(
        "The GiniVisitor is not prepared to visit an int_vector_t");
  }
  // This visitor uses a BinIntNode to split an int_vector_t
  // The splitting value is calculated as the average of the features
  int_feature_t v = 0;
  for (const auto &i : validIndexes_) {
    v += intVector[i];
  }
  v /= intVector.size();

  if (!bestSplitter_) {
    bestSplitter_ = new BinIntNode(featureIndex_, v);
    auto partitions = bestSplitter_->split(validIndexes_, intVector);
    impurity_ = calculateImpurity(partitions);
    std::swap(partitions, bestPartitions_);
  } else {
    INode *candidate = new BinIntNode(featureIndex_, v);
    auto partitions = candidate->split(validIndexes_, intVector);
    const double candidateImpurity = calculateImpurity(partitions);
    if (candidateImpurity < impurity_) {
      delete bestSplitter_;
      bestSplitter_ = candidate;
      impurity_ = candidateImpurity;
      std::swap(partitions, bestPartitions_);
    } else {
      delete candidate;
    }
  }
  isPreparedToVisit_ = false;
}

void GiniVisitor::operator()(const double_vector_t &doubleVector) {
  if (!isPreparedToVisit_) {
    throw std::runtime_error(
        "The GiniVisitor is not prepared to visit an int_vector_t");
  }
  // This visitor uses a BinDoubleNode to split a double_vector_t
  // The splitting value is calculated as the average of the features
  double_feature_t v = 0;
  for (const auto &i : validIndexes_) {
    v += doubleVector[i];
  }
  v /= doubleVector.size();

  if (!bestSplitter_) {
    bestSplitter_ = new BinDoubleNode(featureIndex_, v);
    auto partitions = bestSplitter_->split(validIndexes_, doubleVector);
    impurity_ = calculateImpurity(partitions);
    std::swap(partitions, bestPartitions_);
  } else {
    INode *candidate = new BinIntNode(featureIndex_, v);
    auto partitions = candidate->split(validIndexes_, doubleVector);
    const double candidateImpurity = calculateImpurity(partitions);
    if (candidateImpurity < impurity_) {
      delete bestSplitter_;
      bestSplitter_ = candidate;
      impurity_ = candidateImpurity;
      std::swap(partitions, bestPartitions_);
    } else {
      delete candidate;
    }
  }
  isPreparedToVisit_ = false;
}

[[nodiscard]] std::pair<INode *, partitions_t >
GiniVisitor::getBestSplitterWithPartitions() const {
  return std::make_pair(bestSplitter_, bestPartitions_);
}

double GiniVisitor::calculateImpurity(
    const std::vector<std::vector<index_t>> &partitions) {
  // Create a table (see par. 4.3.4 of the book) zeroed.
  std::map<label_t, std::vector<frequency_t>> labelToFrequencyInPartition;
  for (const auto &partition : partitions) {
    for (const auto &i : partition) {
      const auto currLabel = labels_[i];
      if (labelToFrequencyInPartition.find(currLabel) ==
          labelToFrequencyInPartition.end()) {
        labelToFrequencyInPartition[currLabel] =
            std::vector<frequency_t>(partitions.size(), 0);
      }
    }
  }
  // Fill the table with label frequencies
  for (std::size_t j = 0; j < partitions.size(); j++) {
    for (const auto &i : partitions[j]) {
      const auto currLabel = labels_[i];
      (labelToFrequencyInPartition[currLabel])[j] += 1;
    }
  }
  // Calculate column sums
  std::size_t totalFrequency = 0;
  std::vector<frequency_t> colSums(partitions.size(), 0);
  for (const auto &pair : labelToFrequencyInPartition) {
    for (std::size_t j = 0; j < partitions.size(); j++) {
      colSums[j] += pair.second[j];
      totalFrequency += pair.second[j];
    }
  }
  // Calculate the column Gini's index.
  std::vector<double> giniByColumn(partitions.size(), 1.0f);
  for (const auto &pair : labelToFrequencyInPartition) {
    for (std::size_t j = 0; j < pair.second.size(); j++) {
      const auto relFreq = static_cast<double>(pair.second[j]) / colSums[j];
      giniByColumn[j] -= relFreq * relFreq;
    }
  }
  // Calculate total impurity
  double ret = 0.0;
  for (std::size_t j = 0; j < partitions.size(); j++) {
    ret += giniByColumn[j] * colSums[j] / totalFrequency;
  }
  return ret;
}
