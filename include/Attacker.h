//
// Created by dg on 23/01/20.
//

#ifndef TREEANT_ATTACKER_H
#define TREEANT_ATTACKER_H

#include "types.h"
#include <forward_list>
#include <set>
#include <unordered_map>

// Forward declarations
class Attacker;
class Dataset;

class Attacker {

public:
  explicit Attacker(const Dataset &dataset, const std::string &json,
                    const cost_t &budget);

  // NOTE: addAttackerRule method where you can have more than one rule per
  //       single instance but the pre conditions must be disjoint.
  //       In this way only one rule can be applied to the instance for each
  //       step.
  // Method having the same signature of python code (see line 257)
  // Assumption: one feature can have more than one attacking rule (see Attacker
  // constructor) but the 'pre' values/intervals must be disjoint
  [[nodiscard]] std::vector<std::pair<record_t, cost_t>>
  attack(const record_t &instance, const index_t &featureId,
         const cost_t &cost) const;

private:
  // Private class
  // see line 179 where the constructor is used
  class AttackerRule {

  public:
    explicit AttackerRule(index_t featureIndexToAttack, const std::set<feature_t>& pre,
                          feature_t post, cost_t cost, bool isNumerical);

    explicit AttackerRule(index_t featureIndexToAttack, const std::vector<feature_t>& pre,
                          feature_t post, cost_t cost, bool isNumerical);

    [[nodiscard]] bool apply(const record_t &instance,
                             record_t &newInstance) const;

    [[nodiscard]] const cost_t &getCost() const;

    [[nodiscard]] bool areDisjoint(const AttackerRule &other) const;


  private:

    explicit AttackerRule(index_t featureIndexToAttack,
                          feature_t post, cost_t cost, bool isNumerical);

    // First: the feature index; second: the possible values of the feature not
    // yet attacked
    index_t featureIndexToAttack_;
    // it is a pair in the python code where featureIndexToAttack_ is first
    std::vector<feature_t> pre_;
    feature_t post_; // featureId is pre_.first
    cost_t cost_{};
    bool isNumerical_{};
  };

  // Private members
  std::unordered_map<index_t, std::forward_list<AttackerRule>> rules_;
  cost_t budget_;
  cost_t eps_ = 1e-7; // Needed for comparison between float
};

#endif // TREEANT_ATTACKER_H
