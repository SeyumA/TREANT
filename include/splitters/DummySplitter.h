//
// Created by dg on 06/11/19.
//

#ifndef TREEANT_DUMMYSPLITTER_H
#define TREEANT_DUMMYSPLITTER_H

#include "ISplitter.h"

class DummySplitter : public ISplitter {

public:
  [[nodiscard]] INode *
  split(const dataset_t &dataset,
        const std::vector<unsigned long> &subset) const override;
};

#endif // TREEANT_DUMMYSPLITTER_H
