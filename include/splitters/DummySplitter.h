//
// Created by dg on 06/11/19.
//

#ifndef TREEANT_DUMMYSPLITTER_H
#define TREEANT_DUMMYSPLITTER_H

#include "ISplitter.h"

class DummySplitter : public ISplitter {

public:
  /**
   * The returned INode pointer must have the proper children, especially in the
   * case of continuous features (integers or floating points for example)
   * @param dataset is the original training set
   * @param subset a part of the dataset
   * @param isLastLevel boolean flag -> are we at the last tree level?
   * @return an INode with the children already there (if not a leaf)
   */
  [[nodiscard]] std::pair<INode *, std::vector<dataset_partition_t>>
  split(const dataset_t &dataset, const dataset_partition_t &subset,
        bool isLastLevel) const override;
};

#endif // TREEANT_DUMMYSPLITTER_H
