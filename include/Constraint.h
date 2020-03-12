//
// Created by dg on 22/02/20.
//

#ifndef TREEANT_CONSTRAINT_H
#define TREEANT_CONSTRAINT_H

#include "types.h"
#include <optional>

class Attacker;

class Constraint {

public:
  explicit Constraint(const record_t &x, const label_t &y, const cost_t &cost,
                      const bool &inequality, const gain_t &bound);

  [[nodiscard]] std::optional<Constraint>
  propagateLeft(const Attacker &attacker, index_t featureId,
                feature_t featureValue, bool isNumerical) const;

  [[nodiscard]] std::optional<Constraint>
  propagateRight(const Attacker &attacker, index_t featureId,
                 feature_t featureValue, bool isNumerical) const;

  void setDirection(char direction);

  [[nodiscard]] label_t getY() const;
  [[nodiscard]] bool getInequality() const;
  [[nodiscard]] gain_t getBound() const;
  [[nodiscard]] char getDirection() const;

private:
  const record_t& x_;
  label_t y_;
  cost_t cost_;  // TODO: check if this is ever used
  bool inequality_; // 0 = 'less than', 1 = 'greater than or equal to'
  gain_t bound_;
  char direction_;

  cost_t calculateMinCost(
      const std::vector<std::pair<record_t, cost_t>> &attacks) const;

};

#endif // TREEANT_CONSTRAINT_H
