//
// Created by dg on 06/11/19.
//

#include <map>

#include "nodes/BooleanNode.h"
#include "nodes/Leaf.h"
#include "splitters/DummySplitter.h"

std::pair<INode *, std::vector<dataset_partition_t>>
DummySplitter::split(const std::shared_ptr<IFeatureVector> &subset,
                     bool isLastLevel) const {
//  // Preliminary checks
//  if (dataset.empty()) {
//    throw std::runtime_error("Dataset passed to DummySplitter is empty");
//  }
//  //
//  if (subset.empty()) {
//    throw std::runtime_error("Subset of the dataset passed to DummySplitter "
//                             "is empty");
//  }
//  //
//  // Detect the number of labels available in order to build the table
//  typedef std::size_t frequency_t;
//  std::map<label_t, frequency_t> detectedLabels;
//  for (const auto &i : subset) {
//    label_t currLabel = dataset[i].second;
//    if (detectedLabels.find(currLabel) == detectedLabels.end()) {
//      detectedLabels[currLabel] = 1;
//    } else {
//      detectedLabels[currLabel] += 1;
//    }
//  }
//  //
//  // Only one label has been detected so we return a Leaf
//  if (detectedLabels.size() == 1) {
//    return std::make_pair(new Leaf(detectedLabels.begin()->first),
//                          std::vector<dataset_partition_t>());
//  }
//  //
//  // If we are at the bottom level we should return a Leaf with the
//  // most frequent label
//  if (isLastLevel) {
//    label_t mostFrequent = detectedLabels.begin()->first;
//    frequency_t higherFrequency = detectedLabels.begin()->second;
//    for (const auto &[l, f] : detectedLabels) {
//      if (f > higherFrequency) {
//        mostFrequent = l;
//        higherFrequency = f;
//      }
//    }
//    return std::make_pair(new Leaf(mostFrequent),
//                          std::vector<dataset_partition_t>());
//  }
//  //
//  const auto firstRecord = dataset[0].first;
//  // We store the impurity measures for each feature choice
//  std::map<double,
//           std::pair<INode*, std::vector<dataset_partition_t>>>
//      impurityToPartitions;
//
//  // Work on each column, i.e. on each feature type
//  for (std::size_t c = 0; c < firstRecord.size(); c++) {
//    // Get the first entry useful to get the feature types
//    const feature_t &headerFeature = firstRecord[c];
//
//    // Handle all the different types of columns (features that we can have)
//    if (std::get_if<bool>(std::addressof(headerFeature))) {
//      // bool case (see feature_t definition, order of types)
//      // Calculate the impurity index for this column of integers
//      std::vector<dataset_partition_t> falseAndTrueSubsets(2);
//      for (const auto &i : subset) {
//        if (!std::get<bool>((dataset[i].first).at(c))) {
//          falseAndTrueSubsets[0].push_back(i);
//        } else {
//          falseAndTrueSubsets[1].push_back(i);
//        }
//      }
//      // TODO: find impurity
//      double impurity = 0.0;
//      impurityToPartitions[impurity] =
//          std::make_pair(new Leaf(0), falseAndTrueSubsets);
//
//    } else if (std::get_if<std::int32_t>(std::addressof(headerFeature))) {
//      // std::int32_t case
//
//    } else if (std::get_if<double>(std::addressof(headerFeature))) {
//      // double
//
//    } else {
//      throw std::runtime_error("Missing feature types in DummySplitter");
//    }
//  }  // End of loop over the columns
//  //
//  // Pick the one with the smallest impurity
//  auto it = impurityToPartitions.begin();
//  auto res = it->second;
//  ++it;
//  // Release memory in order to avoid memory leaks
//  while (it != impurityToPartitions.end()) {
//    delete it->second.first;
//    it++;
//  }
  return std::make_pair(nullptr, std::vector<dataset_partition_t>());
}