//
// Created by dg on 23/01/20.
//

#include "Attacker.h"

std::vector<fp_feature_t> Attacker::attack(const std::string &featureName,
                                           const fp_feature_t &originalValue,
                                           const cost_t &cost) const {

  // TODO: you must implement this, now is equal to have budget = 0
  return std::vector<fp_feature_t>(1, originalValue);
}

std::vector<ct_feature_t> Attacker::attack(const std::string &featureName,
                                           const ct_feature_t &originalValue,
                                           const cost_t &cost) const {

  // TODO: you must implement this, now is equal to have budget = 0
  return std::vector<ct_feature_t>(1, originalValue);
}