//
// Created by dg on 23/01/20.
//

#include "Attacker.h"

std::vector<feature_t> Attacker::attack(const std::string &featureName,
                                        const feature_t &originalValue,
                                        const cost_t &cost) const {

  // TODO: you must implement this, now is equal to have budget = 0
  return std::vector<feature_t>(1, originalValue);
}