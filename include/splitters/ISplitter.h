//
// Created by dg on 06/11/19.
//

#ifndef TREEANT_ISPLITTER_H
#define TREEANT_ISPLITTER_H

#include "DecisionTree.h"

enum class SplitterTypes { DUMMY = 0x0 };

class ISplitter {

public:
  // Destructor
  virtual ~ISplitter() = 0;

  // Functions
  [[nodiscard]] virtual INode *
  split(const dataset_t &dataset,
        const std::vector<unsigned long> &subset) const = 0;
};

#endif // TREEANT_ISPLITTER_H
