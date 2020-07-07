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
                                     feature_t post, cost_t cost,
                                     bool isNumerical)
    : featureIndexToAttack_(featureIndexToAttack), post_(post), cost_(cost),
      isNumerical_(isNumerical) {}

Attacker::AttackerRule::AttackerRule(index_t featureIndexToAttack,
                                     const std::set<feature_t> &pre,
                                     feature_t post, cost_t cost,
                                     bool isNumerical)
    : AttackerRule(featureIndexToAttack, post, cost, isNumerical) {

  // Perform some checks
  // Does not make sense have an AttackerRule with an empty 'pre' set.
  if (pre.empty()) {
    throw std::runtime_error("The attacker rule must have some values in the "
                             "'pre' set otherwise is useless");
  }
  if (isNumerical) {
    throw std::runtime_error(
        "You should not use this constructor with numerical");
  }
  // Sort the set in order to do properly the set_intersection is not needed
  // because the elements of a set are ordered.
  pre_ = std::vector<feature_t>(pre.begin(), pre.end());
}

Attacker::AttackerRule::AttackerRule(index_t featureIndexToAttack,
                                     const std::vector<feature_t> &pre,
                                     feature_t post, cost_t cost,
                                     bool isNumerical)
    : AttackerRule(featureIndexToAttack, post, cost, isNumerical) {

  // Perform some checks
  // Does not make sense have an AttackerRule with an empty 'pre' set.
  if (pre.empty()) {
    throw std::runtime_error("The attacker rule must have some values in the "
                             "'pre' set otherwise is useless");
  }
  // The 'pre' must contain only two values in ascending order if numerical
  if (!(isNumerical && pre.size() == 2 && pre[0] <= pre[1])) {
    throw std::runtime_error(
        "The attacker rule referring to a numerical "
        "feature must have two components in the 'pre' "
        "set and the first one must be <= to the second one");
  }
  // Sort the set in order to do properly the set_intersection is not needed
  // because the elements of a set are ordered.
  pre_ = std::vector<feature_t>(pre.begin(), pre.end());
}

bool Attacker::AttackerRule::apply(const record_t &instance,
                                   record_t &newInstance) const {
  const auto preValue = instance[featureIndexToAttack_];
  if (isNumerical_ && preValue >= pre_[0] && preValue <= pre_[1]) {
    newInstance = record_t(instance);
    newInstance[featureIndexToAttack_] += post_;
    return true;
  } else if (!isNumerical_) {
    for (const auto &value : pre_) {
      if (value == instance[featureIndexToAttack_]) {
        newInstance = record_t(instance);
        newInstance[featureIndexToAttack_] = post_;
        return true;
      }
    }
  }
  return false;
}

const cost_t &Attacker::AttackerRule::getCost() const { return cost_; }

bool Attacker::AttackerRule::areDisjoint(const AttackerRule &other) const {

  const auto &preToTest = other.pre_;
  if (isNumerical_ && other.isNumerical_) {
    return preToTest[0] > pre_[1] || preToTest[1] < pre_[0];
  } else if (!isNumerical_ && !other.isNumerical_) {
    // You do not need to sort the elements: the constructor always put them in
    // ascending order
    std::vector<feature_t> v(pre_.size() + preToTest.size());
    const auto it =
        std::set_intersection(pre_.begin(), pre_.end(), preToTest.begin(),
                              preToTest.end(), v.begin());
    return it == v.begin();
  } else {
    throw std::runtime_error("Cannot say if the sets are disjoint");
  }
}

// Attacker --------------------------------------------------------------------
Attacker::Attacker(const Dataset &dataset, const std::string &json,
                   const cost_t &budget)
    : budget_(budget) {

  static const std::string bigDoublePythonString = "np.inf";
  static const std::string bigDoubleAsString = []() {
    const auto bigDouble = std::numeric_limits<double>::max();
    std::ostringstream ss;
    ss << bigDouble;
    return ss.str();
  }();

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

  for (const auto &attack : attackerDefinition["attacks"].items()) {
    //
    for (const auto &ruleAndTypes : attack.value().items()) {
      const std::string &nameOfTheAttackedFeature = ruleAndTypes.key();
      const auto attackedFeatureIndexOpt =
          dataset.getFeatureIndex(nameOfTheAttackedFeature);
      if (!attackedFeatureIndexOpt.has_value()) {
        std::cout << "WARNING: cannot find a column with name '"
                  << nameOfTheAttackedFeature
                  << "' in the dataset, the attacker rule won't be created\n";
        continue;
      }
      const index_t attackedFeatureIndex = attackedFeatureIndexOpt.value();
      //
      // For each feature name I can have more than one rule
      for (const auto &type : ruleAndTypes.value().items()) {

        std::optional<AttackerRule> ruleToAddOpt =
            [&]() -> std::optional<AttackerRule> {
          const cost_t cost = std::stod(type.value()["cost"].dump());
          if (cost <= eps_ && cost >= -eps_) {
            throw std::runtime_error(
                utils::format("Cannot add rule with feature ID {} because the "
                              "cost is too small in modulus",
                              attackedFeatureIndex));
          }
          const std::string preAsString = type.value()["pre"].dump();
          const std::string postAsString = type.value()["post"].dump();
          const auto isNumericalAsString = type.value()["is_numerical"].dump();
          const bool isNumerical = isNumericalAsString == "true";
          if (isNumerical) {
            auto start = preAsString.find('(');
            start++;
            auto stop = preAsString.find(',', start);
            auto s0 = preAsString.substr(start, stop - start);
            // Check for big number
            auto bigDoublePos = s0.find(bigDoublePythonString);
            if (bigDoublePos != std::string::npos) {
              s0 = s0.replace(bigDoublePos, bigDoublePythonString.size(),
                              bigDoubleAsString);
            }
            const feature_t startRange = std::stod(s0);
            start = stop + 1;
            stop = preAsString.find(')', start);
            auto s1 = preAsString.substr(start, stop - start);
            // Check for big number
            bigDoublePos = s1.find(bigDoublePythonString);
            if (bigDoublePos != std::string::npos) {
              s1 = s1.replace(bigDoublePos, bigDoublePythonString.size(),
                              bigDoubleAsString);
            }
            const feature_t stopRange = std::stod(s1);
            const std::vector<feature_t> pre = {startRange, stopRange};
            const feature_t post = std::stod(postAsString);
            return AttackerRule(attackedFeatureIndex, pre, post, cost,
                                isNumerical);
          } else {
            std::set<feature_t> pre;
            auto start = preAsString.find('\'');
            while (start != std::string::npos) {
              start++;
              auto stop = preAsString.find('\'', start);
              if (stop == std::string::npos) {
                throw std::runtime_error(
                    "Cannot find the end of the feature name");
              }
              const auto currFeatureName =
                  preAsString.substr(start, stop - start);
              const auto stringCodeOpt =
                  dataset.getCategoricalFeatureValue(currFeatureName);
              if (!stringCodeOpt.has_value()) {
                std::cout << "WARNING: Cannot get a valid code for feature '"
                          << currFeatureName
                          << "'. This value will be not put in the 'pre' "
                             "condition of rule regarding feature "
                          << nameOfTheAttackedFeature << std::endl;
              } else {
                const auto insertionSuccess = pre.insert(stringCodeOpt.value());
                if (!insertionSuccess.second) {
                  throw std::runtime_error(utils::format(
                      "Invalid attacker rule definition for the "
                      "categorical feature {}: "
                      "there are duplicate entries in the 'pre' set",
                      nameOfTheAttackedFeature));
                }
              }
              start = preAsString.find('\'', stop + 1);
            }
            // Cannot build an Attacker rule with an empty 'pre'
            if (pre.empty()) {
              return std::nullopt;
            }
            const auto postAsStringTrimmed =
                postAsString.substr(1, postAsString.length() - 2);
            const auto postOpt =
                dataset.getCategoricalFeatureValue(postAsStringTrimmed);
            if (!postOpt.has_value()) {
              std::cout << "Cannot get a valid code for feature '"
                        << postAsString << "'" << std::endl;
              return std::nullopt;
            }
            return AttackerRule(attackedFeatureIndex, pre, postOpt.value(),
                                cost, isNumerical);
          }
        }();
        // Add the rule to the map
        if (ruleToAddOpt.has_value()) {
          if (rules_.find(attackedFeatureIndex) != rules_.end()) {
            for (const auto &r : rules_.at(attackedFeatureIndex)) {
              // disjunction test
              if (!r.areDisjoint(ruleToAddOpt.value())) {
                throw std::runtime_error(
                    "Cannot add this rule because 'pre' sets are not disjoint");
              }
            }
            rules_.at(attackedFeatureIndex).push_front(ruleToAddOpt.value());
          } else {
            rules_[attackedFeatureIndex] = {ruleToAddOpt.value()};
          }
        }
      }
    }
  }
}

std::vector<std::pair<record_t, cost_t>>
Attacker::attack(const record_t &instance, const index_t &featureId,
                 const cost_t &instanceCost) const {

  if (instanceCost > budget_) {
    throw std::runtime_error("Cannot attack an instance already out-of-budget");
  }
  // instanceCost is the cost paid so far to get to this instance,
  // so the cost of the attack 0, or better remains instanceCost
  std::vector<std::pair<record_t, cost_t>> ret;
  // The first one is always the input
  ret.emplace_back(instance, instanceCost);
  if (rules_.find(featureId) != rules_.end()) {
    // find among the rules the ONLY ONE that can be applied
    // (it is the only one because the pre conditions are disjoint)
    bool canBeAttacked = true;
    while (canBeAttacked) {
      record_t newInstance;
      canBeAttacked = false;
      const auto &[lastAttacked, lastCost] = *ret.rbegin();
      for (const auto &r : rules_.at(featureId)) {
        const auto possibleCost = lastCost + r.getCost();
        if (possibleCost <= budget_ && r.apply(lastAttacked, newInstance)) {
          ret.emplace_back(newInstance, possibleCost);
          canBeAttacked = true;
          break;
        }
      }
    }
  }
  return ret;
}
