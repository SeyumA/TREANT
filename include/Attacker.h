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
  [[nodiscard]] std::vector<std::pair<record_t, cost_t>>
  attack(const record_t &instance, const feature_t &featureId,
         const cost_t &cost) const;

private:
  // Private class
  // see line 179 where the constructor is used
  class AttackerRule {

  public:
    explicit AttackerRule(std::pair<index_t, std::set<feature_t>> pre,
                          std::pair<index_t, feature_t> post, cost_t cost,
                          bool isNumerical);

    [[nodiscard]] index_t getTargetFeatureIndex() const;

    [[nodiscard]] bool isApplicable(const index_t &featureId,
                                    const feature_t &featureValue) const;

    [[nodiscard]] record_t apply(const record_t &instance) const;

    [[nodiscard]] const cost_t& getCost() const;

  private:
    // First: the feature index; second: the possible values of the feature not
    // yet attacked
    std::pair<index_t, std::set<feature_t>> pre_;
    std::pair<index_t, feature_t> post_;
    cost_t cost_{};
    bool isNumerical_{};
  };

  // Private members
  std::vector<AttackerRule> rules_;
  cost_t budget_;

  // Private functions
  [[nodiscard]] bool isFeatureAttackable(const feature_t &featureId) const;

  [[nodiscard]] std::vector<std::pair<record_t, cost_t>>
  computeAttack(const record_t &instance, const feature_t &featureId,
                const cost_t &cost) const;
};

#endif // TREEANT_ATTACKER_H
