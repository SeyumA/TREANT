//
// Created by dg on 23/01/20.
//

#ifndef TREEANT_ATTACKER_H
#define TREEANT_ATTACKER_H

#include "types.h"
#include <set>
#include <unordered_map>

// Forward declarations
class Attacker;
class Dataset;

class Attacker {

public:
  explicit Attacker(const Dataset &dataset, const std::string &json,
                    const cost_t &budget);

  // self.attacks (see AttackerRule class in the python file)
  // is a cache where the attacker pre-computes the possible attacks to an
  // instance. This function generates on-the-fly the attacks.
  // Assumption: one feature can have maximum one attacking rule (see Attacker constructor)
  [[nodiscard]] std::vector<std::pair<record_t, cost_t>>
  attack(const record_t &instance) const;

  // Method having the same signature of python code (see line 257)
  // Assumption: one feature can have maximum one attacking rule (see Attacker constructor)
  [[nodiscard]] std::vector<std::pair<record_t, cost_t>>
  attack(const record_t &instance, const index_t &featureId, const cost_t &cost) const;

private:
  // Private class
  // see line 179 where the constructor is used
  class AttackerRule {

  public:
    explicit AttackerRule(index_t featureIndexToAttack, std::set<feature_t> pre,
                          feature_t post, cost_t cost, bool isNumerical);

    [[nodiscard]] index_t getTargetFeatureIndex() const;

    [[nodiscard]] record_t apply(const record_t &instance) const;

    [[nodiscard]] const cost_t &getCost() const;

  private:
    // First: the feature index; second: the possible values of the feature not
    // yet attacked
    index_t featureIndexToAttack_;
    // it is a pair in the python code where featureIndexToAttack_ is first
    std::set<feature_t> pre_;
    feature_t post_; // featureId is pre_.first
    cost_t cost_{};
    bool isNumerical_{};
  };

  // Private members
  std::unordered_map<index_t, AttackerRule> rules_;
  cost_t budget_;
  cost_t eps_ = 1e-7; // Needed for comparison

  // Private functions
  void attackRic(const indexes_t &featureIdsToAttack,
                 std::vector<std::pair<record_t, cost_t>> &accumulator) const;
};

#endif // TREEANT_ATTACKER_H
