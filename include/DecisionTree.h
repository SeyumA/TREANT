//
// Created by dg on 01/11/19.
//

#ifndef TREEANT_DECISIONTREE_H
#define TREEANT_DECISIONTREE_H

#include <iosfwd>
#include <vector>

#include "Dataset.h"
#include "nodes/INode.h"
#include "splitters/ISplitter.h"

class DecisionTree final {

public:
  enum class VisitorConstructorTypes { GINI = 0x0 };

  // Constructors
  DecisionTree() = delete;
  DecisionTree(DecisionTree &) = delete;
  DecisionTree(DecisionTree &&) = delete;

  explicit DecisionTree(const Dataset &dataset, const std::size_t &maxDepth,
                        VisitorConstructorTypes visitorType);

  // Destructor
  ~DecisionTree();

  // Functions
  [[nodiscard]] label_t predict(const record_t &) const;
  std::size_t getHeight() const;

  friend std::ostream &operator<<(std::ostream &os, const DecisionTree &dt);

private:
  std::size_t height_ = 0;
  INode *root_ = nullptr;
};

std::ostream &operator<<(std::ostream &os, const DecisionTree &dt);

#endif // TREEANT_DECISIONTREE_H
