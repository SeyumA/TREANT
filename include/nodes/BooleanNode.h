//
// Created by dg on 06/11/19.
//

#ifndef TREEANT_BOOLEANNODE_H
#define TREEANT_BOOLEANNODE_H

#include "INode.h"
#include <stdexcept>

class BooleanNode : public INode {
public:
  // Constructors
  explicit BooleanNode(std::size_t featureIndex);
  // Destructor
  ~BooleanNode() override;

  [[nodiscard]] std::vector<INode *> getChildren() const override;
  void setChild(std::size_t index, INode *newNodePtr) override;
  void setFeatureIndex(std::size_t index) override;

  // This function is useful for building a decision tree.
  [[nodiscard]] std::vector<std::vector<index_t>>
  split(const std::vector<index_t> &validIndexes,
        const feature_vector_t &featureVector) override {
    const auto boolVectorPtr = std::get_if<bool_vector_t>(&featureVector);
    if (!boolVectorPtr) {
      throw std::runtime_error(
          "BooleanNode::split() called on a non-bool_vector_t");
    }

    std::vector<std::vector<index_t>> p(2);
    for (const auto &i : validIndexes) {
      if (!(*boolVectorPtr)[i]) {
        p[0].push_back(i);
      } else {
        p[1].push_back(i);
      }
    }

    return p;
  }

  // This function is useful to use the decision tree. It should call
  // recursively predict on the next child.
  [[nodiscard]] label_t predict(const record_t &record) const override {
    const auto boolFeaturePtr =
        std::get_if<bool_feature_t>(&record[featureIndex_]);
    if (!boolFeaturePtr) {
      throw std::runtime_error(
          "BooleanNode::predict() called on a non-bool_feature_t");
    } else if (*boolFeaturePtr) {
      return leftChild->predict(record);
    } else {
      return rightChild->predict(record);
    }
  }

private:
  index_t featureIndex_;
  INode *leftChild = nullptr;
  INode *rightChild = nullptr;
};

#endif // TREEANT_BOOLEANNODE_H
