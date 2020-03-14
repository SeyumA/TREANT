//
// Created by dg on 23/01/20.
//

#include "Attacker.h"
#include "Dataset.h"
#include "nlohmann/json.hpp"
#include "utils.h"

#include <fstream>
#include <queue>
// TODO: delete this iostream after debugging
#include <iostream>
#include <stack>

// AttackerRule ----------------------------------------------------------------
Attacker::AttackerRule::AttackerRule(index_t featureIndexToAttack,
                                     std::set<feature_t> pre, feature_t post,
                                     cost_t cost, bool isNumerical)
    : featureIndexToAttack_(featureIndexToAttack), pre_(std::move(pre)),
      post_(std::move(post)), cost_(cost), isNumerical_(isNumerical) {

  // Perform some checks
  // Does not make sense have an AttackerRule with an empty 'pre' set.
  if (pre.empty()) {
    throw std::runtime_error("The attacker rule must have some values in the "
                             "'pre' set otherwise is useless");
  }
  // The 'pre' must contain only two values in ascending order if numerical
  if (!(isNumerical && pre.size() != 2 &&
        *(pre.begin()) <= *(std::next(pre.begin())))) {
    throw std::runtime_error(
        "The attacker rule referring to a numerical "
        "feature must have two components in the 'pre' "
        "set and the first one must be <= to the second one");
  }
}

index_t Attacker::AttackerRule::getTargetFeatureIndex() const {
  return featureIndexToAttack_;
}

bool Attacker::AttackerRule::apply(const record_t &instance,
                                   record_t &newInstance) const {
  const auto preValue = instance[featureIndexToAttack_];
  if (isNumerical_ && preValue >= *pre_.begin() &&
      preValue <= *(std::next(pre_.begin()))) {
    newInstance = record_t(instance);
    newInstance[featureIndexToAttack_] += post_;
    return true;
  } else if (!isNumerical_ &&
             pre_.find(instance[featureIndexToAttack_]) != pre_.end()) {
    newInstance = record_t(instance);
    newInstance[featureIndexToAttack_] = post_;
    return true;
  }
  return false;
}

const cost_t &Attacker::AttackerRule::getCost() const { return cost_; }
bool Attacker::AttackerRule::areDisjoint(const std::set<feature_t>& preToTest) const{
return false;
}

// Attacker --------------------------------------------------------------------
Attacker::Attacker(const Dataset &dataset, const std::string &json,
                   const cost_t &budget)
    : budget_(budget) {

  const nlohmann::json attackerDefinition = [&json]() {
    std::ifstream ifs;
    ifs.open(json);
    if (!ifs.is_open() || !ifs.good()) {
      throw std::runtime_error(
          "The attacker file stream is not open or not good");
    }
    std::string line;
    std::string attackerDefinitionAsString;
    while (std::getline(ifs, line, '\n')) {
      attackerDefinitionAsString += line;
    }
    return nlohmann::json::parse(attackerDefinitionAsString);
  }();

  for (const auto &[key, value] : attackerDefinition["attacks"].items()) {
    const index_t attackedFeatureIndex = dataset.getFeatureIndex(key);
    // get pre
    const bool isNumerical = value["is_numerical"] == "true" ? true : false;
    const std::string preAsString = value["pre"].dump();
    const cost_t cost = std::stod(value["cost"].dump());
    if (cost <= eps_) {
      throw std::runtime_error(utils::format(
          "Cannot add rule with feature ID {} because the cost is too small",
          attackedFeatureIndex));
    }
    const feature_t post = std::stod(value["post"].dump());
    if (isNumerical) {
      auto start = preAsString.find('(');
      auto stop = preAsString.find(',', start);
      const feature_t startRange =
          std::stod(preAsString.substr(start, stop - start));
      start = stop + 1;
      stop = preAsString.find(')', start);
      const feature_t stopRange =
          std::stod(preAsString.substr(start, stop - start));
      // TODO: do rules for
      if (rules_.find(attackedFeatureIndex) != rules_.end()) {
        for (const auto& r : rules_.at(attackedFeatureIndex)) {
          r.
        }
        rules_.at(attackedFeatureIndex)
            .push_front(AttackerRule(attackedFeatureIndex,
                                     {startRange, stopRange}, post, cost,
                                     isNumerical));
      }
      const auto success = rules_.emplace(attackedFeatureIndex, );
      if (!success.second) {
        throw std::runtime_error(utils::format(
            "Cannot add rule with feature ID {} because would be a duplicate",
            attackedFeatureIndex));
      }
    } else {
      std::set<feature_t> preValues;
      auto start = preAsString.find('\'');
      while (start != std::string::npos) {
        auto stop = preAsString.find('\'', start);
        const auto currFeatureName = preAsString.substr(start, stop - start);
        const auto insertResult = preValues.insert(
            dataset.getCategoricalFeatureValue(currFeatureName));
        if (!insertResult.second) {
          throw std::runtime_error(
              utils::format("Invalid attacker rule definition for the "
                            "categorical feature {}: "
                            "there are duplicate entries in the 'pre' set",
                            key));
        }
      }

      const auto success = rules_.emplace(
          attackedFeatureIndex, AttackerRule(attackedFeatureIndex, preValues,
                                             post, cost, isNumerical));
      if (!success.second) {
        throw std::runtime_error(utils::format(
            "Cannot add rule with feature ID {} because would be a duplicate",
            attackedFeatureIndex));
      }
    }
  }
}

std::vector<std::pair<record_t, cost_t>>
Attacker::attack(const record_t &instance) const {

  std::vector<index_t> featuresToAttack;
  for (const auto &r : rules_) {
    featuresToAttack.push_back(r.first);
  }

  std::vector<std::pair<record_t, cost_t>> ret;
  ret.emplace_back(instance, 0.0);

  // TODO: Check with Prof. Lucchese why we return a 0.0 + cost
  //  (see line 281 'attacks_xf = [ (x,0+cost)]' of python code)
  // TODO: build a cache of the attacks?

  attackRic(featuresToAttack, ret);
  return ret;
}

void Attacker::attackRic(
    const indexes_t &featureIdsToAttack,
    std::vector<std::pair<record_t, cost_t>> &accumulator) const {

  throw std::runtime_error("To be implemented");
  //
  //  // Get the last instance inserted, it is our basis
  //  const auto &[instance, currentCost] = accumulator.back();
  //
  //  // Generate the 2^n possible attacked instances, where n = validIds.size()
  //  std::queue<
  //      std::tuple<std::vector<double>, std::vector<bool>, double, unsigned>>
  //      q;
  //  q.push(std::make_tuple(instance,
  //                         std::vector<bool>(featureIdsToAttack.size(),
  //                         false), currentCost, 0));
  //  while (!q.empty()) {
  //    const auto [attackedInstance, attackedActiveVector, attackedCost,
  //                firstIdToAttack] = q.front();
  //    q.pop();
  //    for (unsigned i = firstIdToAttack; i < attackedActiveVector.size(); i++)
  //    {
  //      const auto featureIdToAttack = featureIdsToAttack[i];
  //      // TODO: find the rule to apply
  //      const auto cost = rules_.at(featureIdToAttack).getCost();
  //      const auto expectedCost = attackedCost + cost;
  //      if (expectedCost <= eps_ + budget_) {
  //        // Create the new instance
  //
  //        // TODO: find the rule to apply
  //        std::vector<double> newInstance =
  //            rules_.at(featureIdToAttack).apply(instance);
  //        // Update the queue
  //        std::vector<bool> newAttackedActiveVector(attackedActiveVector);
  //        newAttackedActiveVector[i] = true;
  //        q.push(std::make_tuple(newInstance, newAttackedActiveVector,
  //                               expectedCost, i + 1));
  //        accumulator.emplace_back(newInstance, expectedCost);
  //        // Print the new feature added
  //        std::cout << "{ ";
  //        for (const auto &d : newInstance) {
  //          std::cout << d << " ";
  //        }
  //        std::cout << "} cost: " << expectedCost << std::endl;
  //        // Prepare the new feature ids to attack
  //        indexes_t newFeatureIdsToAttack;
  //        for (unsigned ii = 0; ii < newAttackedActiveVector.size(); ii++) {
  //          if (newAttackedActiveVector[ii]) {
  //            newFeatureIdsToAttack.emplace_back(featureIdsToAttack[ii]);
  //          }
  //        }
  //        // recursive call
  //        attackRic(newFeatureIdsToAttack, accumulator);
  //      }
  //    } // end of for loop for children generation
  //  }   // end of recursion on queue
}

std::vector<std::pair<record_t, cost_t>>
Attacker::attack(const record_t &instance, const index_t &featureId,
                 const cost_t &instanceCost) const {

  // instanceCost is the cost paid so far to get to this instance,
  // so the cost of the attack 0, or better remains instanceCost
  std::vector<std::pair<record_t, cost_t>> ret;
  if (rules_.find(featureId) == rules_.end()) {
    // TODO: check with Prof. Lucchese if the cost must be 0.0 or 'cost', I
    // would expect 0.0
    ret.emplace_back(instance, instanceCost);
  } else {
    // find among the rules the ONLY ONE that can be applied
    // (it is the only one because the pre conditions are disjoint)
    cost_t currentCost = instanceCost;
    bool canBeAttacked = true;
    while (currentCost <= budget_ && canBeAttacked) {
      record_t newInstance;
      canBeAttacked = false;
      for (const auto &r : rules_.at(featureId)) {
        if (r.apply(instance, newInstance)) {
          currentCost += r.getCost();
          ret.emplace_back(newInstance, currentCost);
          canBeAttacked = true;
          break;
        }
      }
    }
  }
  return ret;
}
