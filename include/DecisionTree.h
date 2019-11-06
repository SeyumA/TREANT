//
// Created by dg on 01/11/19.
//

#ifndef TREEANT_DECISIONTREE_H
#define TREEANT_DECISIONTREE_H

#include <vector>
#include <memory>  // shared_ptr

#include "nodes/INode.h"
#include "splitters/ISplitter.h"


class ISplitter;

class DecisionTree final {

public:
  // Constructors
  explicit DecisionTree(const dataset_t &dataset, const std::size_t &maxDepth);
  // Destructor
  ~DecisionTree();

  // Functions
  static std::pair<INode *, std::size_t>
  buildRecursively(const dataset_t &dataset,
                   const std::vector<unsigned long> &subset,
                   const ISplitter* splitter, const std::size_t &currDepth,
                   const std::size_t &maxDepth);

  [[nodiscard]] int predict(const record_t &r) const;

private:
  unsigned int depth;
  INode *root = nullptr;
  std::shared_ptr<ISplitter> splitter;
};

#endif // TREEANT_DECISIONTREE_H
