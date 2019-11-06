//
// Created by dg on 06/11/19.
//

#include "splitters/DummySplitter.h"

INode * DummySplitter::split(const dataset_t &dataset,
             const std::vector<unsigned long> &subset) const {

  // Preliminary checks
  if (dataset.empty()) {

  } else {
    for (const auto& i : subset) {
      dataset[i];
    }
  }
  // Get the types of the record
  return nullptr;
}