//
// Created by dg on 06/11/19.
//

#ifndef TREEANT_ISPLITTER_H
#define TREEANT_ISPLITTER_H

#include "DecisionTree.h"

typedef std::vector<unsigned long> dataset_partition_t;

enum class SplitterTypes { DUMMY = 0x0 };

class ISplitter {
public:
  // Destructor
  virtual ~ISplitter() = default;

  // Functions
  [[nodiscard]] virtual std::pair<INode *, std::vector<dataset_partition_t>>
  split(const dataset_t &dataset, const std::vector<unsigned long> &subset,
        bool isLastLevel) const = 0;
};

#endif // TREEANT_ISPLITTER_H
