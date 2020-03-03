//
// Created by dg on 23/01/20.
//

#include "Attacker.h"

std::vector<std::pair<record_t, cost_t>>
Attacker::attack(const record_t &instance, const feature_t &featureId,
                 const cost_t &cost) const {

  // TODO: you must implement this, now is equal to have budget = 0
  std::vector<std::pair<record_t, cost_t>> ret = {
      std::make_pair(instance, 0.0)};
  return ret;
}