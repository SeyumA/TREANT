//
// Created by dg on 18/11/19.
//

// STD library
#include <map>
// Local headers
#include "visitors/GiniVisitor.h"
#include "nodes/BinDoubleNode.h"
#include "nodes/BinIntNode.h"
#include "nodes/BooleanNode.h"

GiniVisitor::GiniVisitor(const std::vector<index_t> &validIndexes,
                         const std::vector<label_t> &labels)
    : featureIndex_(0), validIndexes_(validIndexes), labels_(labels),
      bestSplitter_(nullptr), impurity_(0.0) {}

void GiniVisitor::operator()(const bool_vector_t &boolVector) {
  // This visitor uses a BooleanNode to split a bool_vector_t
  INode *candidate = new BooleanNode(featureIndex_);
  auto partitions = candidate->split(validIndexes_, boolVector);

  tryAssignNode(candidate, partitions);
}

void GiniVisitor::operator()(const int_vector_t &intVector) {
  // This visitor uses a BinIntNode to split an int_vector_t
  // The splitting value is calculated as the average of the features
  int_feature_t v = 0;
  for (const auto &i : validIndexes_) {
    v += intVector[i];
  }
  v /= validIndexes_.size();

  INode *candidate = new BinIntNode(featureIndex_, v);
  auto partitions = candidate->split(validIndexes_, intVector);

  tryAssignNode(candidate, partitions);
}

void GiniVisitor::operator()(const double_vector_t &doubleVector) {
  // This visitor uses a BinDoubleNode to split a double_vector_t
  // The splitting value is calculated as the average of the features
  double_feature_t v = 0;
  for (const auto &i : validIndexes_) {
    v += doubleVector[i];
  }
  v /= validIndexes_.size();

  INode *candidate = new BinDoubleNode(featureIndex_, v);
  auto partitions = candidate->split(validIndexes_, doubleVector);

  tryAssignNode(candidate, partitions);
}

std::pair<INode *, partitions_t>
GiniVisitor::getBestSplitterWithPartitions() const {
  return std::make_pair(bestSplitter_, bestPartitions_);
}

IFeatureVectorVisitor *GiniVisitor::clone() const {
  return new GiniVisitor(this->validIndexes_, this->labels_);
}

IFeatureVectorVisitor *
GiniVisitor::clone(const std::vector<index_t> &validIndexes) const {
  return new GiniVisitor(validIndexes, this->labels_);
}

[[nodiscard]] const std::vector<index_t> &GiniVisitor::getValidIndexes() const {
  return validIndexes_;
}

void GiniVisitor::visitFeatureVectors(
    const std::vector<feature_vector_t> &featureVectors) {
  for (index_t i = 0; i < featureVectors.size(); i++) {
    // N.B. : The update of featureIndex_ is crucial
    this->featureIndex_ = i;
    std::visit(*this, featureVectors[i]);
  }
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
      const auto relFreq =
          colSums[j] ? static_cast<double>(pair.second[j]) / colSums[j] : 0;
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

void GiniVisitor::tryAssignNode(INode *candidate, partitions_t &partitions) {
  if (!bestSplitter_) {
    bestSplitter_ = candidate;
    impurity_ = calculateImpurity(partitions);
    std::swap(partitions, bestPartitions_);
  } else {
    const double candidateImpurity = calculateImpurity(partitions);
    if (candidateImpurity < impurity_) {
      delete bestSplitter_;
      bestSplitter_ = candidate;
      impurity_ = candidateImpurity;
      std::swap(partitions, bestPartitions_);
    } else {
      delete candidate;
      candidate = nullptr;
    }
  }
}
