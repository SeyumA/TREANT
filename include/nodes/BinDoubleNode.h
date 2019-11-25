//
// Created by dg on 25/11/19.
//

#ifndef TREEANT_BINDOUBLENODE_H
#define TREEANT_BINDOUBLENODE_H

#include <stdexcept>
#include "nodes/INode.h"

class BinDoubleNode : public INode {
public:
  explicit BinDoubleNode(index_t featureIndex, double_feature_t v,
                         INode *leftChild, INode *rightChild);

  ~BinDoubleNode() override;

  [[nodiscard]] std::vector<INode *> getChildren() const override;
  void setChild(std::size_t index, INode *newNodePtr) override;
  void setFeatureIndex(std::size_t index) override;

  // This function is useful for building a decision tree.
  [[nodiscard]] std::vector<std::vector<index_t>>
  split(const std::vector<index_t> &validIndexes,
        const feature_vector_t &featureVector) override {
    const auto doubleVectorPtr = std::get_if<double_vector_t>(&featureVector);
    if (!doubleVectorPtr) {
      throw std::runtime_error(
          "BinDoubleNode::split() called on a non-double_vector_t");
    }
    // Set v as the average of the vector
    v = 0.0;
    for (const auto& i : validIndexes) {
      v += (*doubleVectorPtr)[i];
    }
    v /= validIndexes.size();
    // Create the 2 partitions
    std::vector<std::vector<index_t>> p(2);
    for (const auto& i : validIndexes) {
      if ((*doubleVectorPtr)[i] > v) {
        p[1].push_back(i);
      } else {
        p[0].push_back(i);
      }
    }
    //
    return p;
  }

  // This function is useful to use the decision tree. It should call
  // recursively predict on the next child.
  [[nodiscard]] label_t predict(const record_t &record) const override {
    const auto doubleFeaturePtr = std::get_if<double_feature_t>(&record[featureIndex_]);
    if (!doubleFeaturePtr) {
      throw std::runtime_error("BinDoubleNode::predict() called on a non-double_feature_t");
    }
    else if (*doubleFeaturePtr > v) {
      return rightChild->predict(record);
    } else {
      return leftChild->predict(record);
    }
  }

private:
  index_t featureIndex_;
  double_feature_t v;
  INode *leftChild = nullptr;
  INode *rightChild = nullptr;


};

#endif // TREEANT_BINDOUBLENODE_H
