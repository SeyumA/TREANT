//
// Created by dg on 05/11/19.
//

#ifndef TREEANT_INODE_H
#define TREEANT_INODE_H

#include <string>
#include <vector>

#include "types.h"

/*
// The dynamic_cast will be used by all the nodes
#include <iostream>
#include <cassert>

struct V {
    virtual ~V() = default;
    virtual void f() const {
        std::cout << "V::f() called" << std::endl;
    };  // must be polymorphic to use runtime-checked dynamic_cast
};
struct A : virtual V {
    void f() const override {
        std::cout << "A::f() called" << std::endl;
    }
};

void print(V* v_from_a) {
    if (A* recast_v_from_a = dynamic_cast<A*>(v_from_a)) {
        std::cout << "Calling f() on A* a" << std::endl;

        recast_v_from_a->f();
        std::cout << std::endl;
    } else {
        std::cout << "print cannot be called\n";
    }
}

int main()
{
    V* av = new A;
    print(av);
    delete av;

    V* v = new V;
    print(v);
    delete v;
}
*/

class INode {
public:
  virtual ~INode() = 0;
  // TODO: accept(IVisitor* visitor)

  //[[nodiscard]] virtual int predict(const DataSubset &dataSubset) const = 0;

  [[nodiscard]] virtual std::vector<INode*> getChildren() const = 0;
  virtual void setChild(std::size_t index, INode* newNodePtr) = 0;
  virtual void setFeatureIndex(std::size_t index) = 0;
};

#endif // TREEANT_INODE_H

