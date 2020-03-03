//
// Created by dg on 23/01/20.
//

#ifndef TREEANT_ATTACKER_H
#define TREEANT_ATTACKER_H

#include "types.h"

class Attacker {

public:
  [[nodiscard]] std::vector<std::pair<record_t, cost_t>>
  attack(const record_t &x, const feature_t &featureId,
         const cost_t &cost) const;

private:

  class AttackerRule {

    // TODO: 24 feb 2020 continue with the attacker rule

  private:
//    std::map<std::tuple<>>

  };

  std::vector<AttackerRule> rules_;

  // self.attacks (see AttackerRule class in the python file)
  // is a cache where the attacker precomputes the possible attacks to an instance

};

#endif // TREEANT_ATTACKER_H
