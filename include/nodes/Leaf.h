//
// Created by dg on 05/11/19.
//

#ifndef TREEANT_LEAF_H
#define TREEANT_LEAF_H

#include "INode.h"

class Leaf : public INode {
public:
  // Constructors
  explicit Leaf(int label);
  // Functions
  [[nodiscard]] int predict(record_t) const override;

private:
  int label;
};


#endif // TREEANT_LEAF_H
