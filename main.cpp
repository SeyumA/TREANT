#include <iostream>
#include <iterator>
#include <regex>

#include "Dataset.h"
#include "DecisionTree.h"
#include "Logger.h"
#include "OptParser.h"
#include "features/BoolFeatureVector.h"
#include "features/DoubleFeatureVector.h"
#include "features/IntFeatureVector.h"
#include "splitters/DummySplitter.h"
#include "visitors/ImpurityVisitor.h"

struct IFVector;
struct IntFVector;
struct BoolFVector;

struct IBuildingVisitor {
  virtual ~IBuildingVisitor() = default;

  virtual void visit(const IntFVector *featureVector,
                     const indexes_t& validIndexes) = 0;

  virtual void visit(const BoolFVector *featureVector,
                     const indexes_t& validIndexes) = 0;
};

struct DummyBuildingVisitor : public IBuildingVisitor {
  friend class IntFVector;
  friend class BoolFVector;

//  void visit(const IFVector *featureVector,
//             const indexes_t& validIndexes) {
//    std::cout << "visit overloading IFVector\n";
//  }

  void visit(const IntFVector *intFVector,
             const indexes_t& validIndexes) {
    std::cout << "visit overloading IntFVector\n";


  }

  void visit(const BoolFVector *boolFVector,
             const indexes_t& validIndexes) {
    std::cout << "visit overloading BoolFVector\n";
  }
};

struct IFVector {
  virtual ~IFVector() = default;
  virtual void accept(IBuildingVisitor* visitor,
                      const indexes_t& validIndexes) = 0;
};

struct BoolFVector : public IFVector {
  void accept(IBuildingVisitor* visitor,
              const indexes_t& validIndexes) {
    visitor->visit(this, validIndexes);
  }
};

struct IntFVector : public IFVector {
  void accept(IBuildingVisitor* visitor,
              const indexes_t& validIndexes) {
    visitor->visit(this, validIndexes);
  }
};


int main() {
  std::cout << "Start of the program" << std::endl;

  //Dataset myDataset("", "");

  {
    IBuildingVisitor* visitor = new DummyBuildingVisitor{};
    IFVector* boolFVector = new BoolFVector{};
    IFVector* intFVector = new IntFVector{};
    indexes_t indexes;

    boolFVector->accept(visitor, indexes);

    intFVector->accept(visitor, indexes);

    delete visitor;
    delete boolFVector;
    delete intFVector;
  }


  {
    ImpurityVisitor vis;
  }

  std::cout << "End of program" << std::endl;
  return 0;
}
