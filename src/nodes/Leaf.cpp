//
// Created by dg on 05/11/19.
//

#include "nodes/Leaf.h"

Leaf::Leaf(int label) : label(label) {}

int Leaf::predict(record_t) const { return label; }
