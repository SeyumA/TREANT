//
// Created by dg on 05/11/19.
//

#ifndef TREEANT_INODE_H
#define TREEANT_INODE_H

#include <variant>
#include <vector>
#include <string>

#include "types.h"

class INode {
public:
  virtual ~INode() = 0;
  [[nodiscard]] virtual int predict(record_t r) const = 0;
};

#endif // TREEANT_INODE_H
