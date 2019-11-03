//
// Created by dg on 01/11/19.
//

#ifndef TREEANT_DECISIONTREE_H
#define TREEANT_DECISIONTREE_H

#include <functional>
#include <iostream>
#include <utility>
#include <variant>
#include <vector>

template<class T> struct always_false : std::false_type {};

typedef std::variant<int, std::string> feature_t;
typedef std::vector<feature_t> record_t;

struct RecordVisitor {
  auto operator()(feature_t&& arg) {
    using T = std::decay_t<decltype(arg)>;
    if constexpr (std::is_same_v<T, int>)
      return arg;
    else if constexpr (std::is_same_v<T, std::string>)
      return arg;
    else
      throw std::runtime_error("Invalid feature type");
  }
};

class INode {
public:
  [[nodiscard]] virtual int predict(record_t r) const = 0;
};


struct Leaf : public INode {
  int depth;
  int label;

  explicit Leaf(int depth, int label) : depth(depth), label(label) {

  };

  [[nodiscard]] int predict(record_t) const override {
    return label;
  }
};


// Binary integer node (partition in two parts depending on
class BinIntNode : public INode {
public:
  explicit BinIntNode(int feature_index, int depth, int v,
                      INode* leftChild, INode* rightChild) :
      feature_index(feature_index), depth(depth), v(v), leftChild(leftChild),
      rightChild(rightChild) {
  };

  [[nodiscard]] int predict(record_t r) const override {
    int record_feature = std::get<int>(r[feature_index]);
    return record_feature <= v ? leftChild->predict(r) : rightChild->predict(r);
  }

//private:
  int feature_index;
  int depth;
  int v;
  INode* leftChild = nullptr;
  INode* rightChild = nullptr;
};

class DecisionTree {

public:
  DecisionTree(int depth, INode* root) : depth(depth), root(root) {

  }


//private:
  int depth;
  INode* root = nullptr;
};

#endif // TREEANT_DECISIONTREE_H
