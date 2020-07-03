//
// Created by dg on 01/11/19.
//

#ifndef TREEANT_DECISIONTREE_H
#define TREEANT_DECISIONTREE_H

#include "Constraint.h"
#include "types.h"

#include <iosfwd>
#include <unordered_map>
#include <vector>

class Node;
class Dataset;
class Attacker;

class DecisionTree final {

public:
  // Constructor
  explicit DecisionTree();
  // Destructor
  ~DecisionTree();

  // Load the internal node structure from a file
  void load(const std::string& filePath);
  // Save the internal node structure to a file
  void save(const std::string& filePath) const;

  // Functions
  // Assumption: X is store in row-wise order (C-order)
  void predict(const double *X, const unsigned rows, const unsigned cols,
               double *res, const bool isRowsWise, const bool score) const;

  [[nodiscard]] std::size_t getHeight() const;
  [[nodiscard]] std::size_t getNumberNodes() const;

  void fit(const Dataset &dataset, const std::string &attackerFile,
           const cost_t &budget, const unsigned &threads,
           const bool &useICML2019,
           const unsigned &maxDepth,
           const unsigned minPerNode = 20,
           const bool isAffine = false,
           const Impurity impurityType = Impurity::SSE);

  [[nodiscard]] bool isTrained() const;

  friend std::ostream &operator<<(std::ostream &os, const DecisionTree &dt);

private:
  Node *root_ = nullptr;

  Node *fitRecursively(const Dataset &dataset, const indexes_t &rows,
                       const indexes_t &validFeatures, std::size_t currHeight,
                       const Attacker &attacker,
                       const std::unordered_map<index_t, cost_t> &costs,
                       const prediction_t &currentPrediction,
                       const unsigned &maxDepth,
                       const unsigned minPerNode,
                       const bool isAffine,
                       Impurity impurityType,
                       const std::vector<Constraint> &constraints,
                       const unsigned &threads, const bool &useICML2019) const;
};

std::ostream &operator<<(std::ostream &os, const DecisionTree &dt);

#endif // TREEANT_DECISIONTREE_H
