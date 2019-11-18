//
// Created by dg on 06/11/19.
//

#ifndef TREEANT_ISPLITTER_H
#define TREEANT_ISPLITTER_H

#include "DecisionTree.h"
#include <features/IFeatureVector.h>

typedef std::vector<unsigned long> dataset_partition_t;

class ISplitter {
public:
  // Destructor
  virtual ~ISplitter() = default;

  // Functions
  [[nodiscard]] virtual std::pair<INode *, std::vector<dataset_partition_t>>
  split(const std::shared_ptr<IFeatureVector> &subset,
        bool isLastLevel) const = 0;
};

#endif // TREEANT_ISPLITTER_H
