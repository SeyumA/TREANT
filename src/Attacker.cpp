//
// Created by dg on 23/01/20.
//

#include "Attacker.h"
#include "Dataset.h"
#include "nlohmann/json.hpp"
#include "utils.h"
#include <fstream>
// TODO: delete this iostream after debugging
#include <iostream>
#include <stack>

// AttackerRule ----------------------------------------------------------------
Attacker::AttackerRule::AttackerRule(
    std::pair<index_t, std::set<feature_t>> pre,
    std::pair<index_t, feature_t> post, cost_t cost, bool isNumerical)
    : pre_(std::move(pre)), post_(std::move(post)), cost_(cost),
      isNumerical_(isNumerical) {
  // Perform some checks
  if (!(isNumerical && pre.second.size() != 2 &&
        *(pre.second.begin()) <= *(std::next(pre.second.begin())))) {
    throw std::runtime_error(
        "The attacker rule referring to a numerical "
        "feature must have two components in the 'pre' "
        "set and the first one must be <= to the second one");
  }
  // Does not make sense have an AttackerRule with an empty 'pre' set.
  if (pre.second.empty()) {
    throw std::runtime_error("The attacker rule must have some values in the "
                             "'pre' set otherwise is useless");
  }
}

index_t Attacker::AttackerRule::getTargetFeatureIndex() const {
  return pre_.first;
}

bool Attacker::AttackerRule::isApplicable(const index_t &featureId,
                                          const feature_t &featureValue) const {

  return (isNumerical_ && pre_.first == featureId &&
          featureValue >= *(pre_.second.begin()) &&
          featureValue <= *(std::next(pre_.second.begin()))) ||
         (!isNumerical_ && pre_.second.find(featureValue) != pre_.second.end());
}

record_t Attacker::AttackerRule::apply(const record_t &instance) const {
  auto ret = record_t(instance);
  if (isNumerical_) {
    ret[post_.first] += post_.second;
  } else {
    ret[post_.first] = post_.second;
  }
  return ret;
}

const cost_t &Attacker::AttackerRule::getCost() const { return cost_; }

// Attacker --------------------------------------------------------------------
Attacker::Attacker(const Dataset &dataset, const std::string &json,
                   const cost_t &budget)
    : budget_(budget) {

  const nlohmann::json attackerDefinition = [&json](){
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
      rules_.push_back(
          AttackerRule({attackedFeatureIndex, {startRange, stopRange}},
                       {attackedFeatureIndex, post}, cost, isNumerical));
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
      rules_.push_back(AttackerRule({attackedFeatureIndex, preValues},
                                    {attackedFeatureIndex, post}, cost,
                                    isNumerical));
    }
  }
}

std::vector<std::pair<record_t, cost_t>>
Attacker::attack(const record_t &instance, const feature_t &featureId,
                 const cost_t &cost) const {

  // This is actually an implementation of __compute_attacks
  if (!isFeatureAttackable(featureId)) {
    // TODO: Check with Prof. Lucchese why we return a 0.0 + cost
    //  (see line 281 'attacks_xf = [ (x,0+cost)]' of python code)
    return {std::make_pair(instance, cost)};
  } else {
    // TODO: build a cache of the attacks?
    return computeAttack(instance, featureId, cost);
  }
}

bool Attacker::isFeatureAttackable(const feature_t &featureId) const {

  for (const auto &rule : rules_) {
    if (rule.getTargetFeatureIndex() == featureId) {
      return true;
    }
  }
  return false;
}

std::vector<std::pair<record_t, cost_t>>
Attacker::computeAttack(const record_t &instance, const feature_t &featureId,
                        const cost_t &cost) const {

  const auto isEqualPerturbation = [](const std::pair<record_t, cost_t> &a,
                                      const std::pair<record_t, cost_t> &b) {
    const auto n = a.first.size();
    if (n != b.first.size()) {
      throw std::runtime_error("Cannot use isEqualPerturbation lambda");
    }
    if (a.second > b.second) {
      return false;
    }
    // Check on the record
    for (index_t i = 0; i < n; i++) {
      if (a.first[i] != b.first[i]) {
        return false;
      }
    }
    // Return true if a.first is equal to b.first and a.second <= b.second
    return true;
  };

  std::vector<std::pair<record_t, cost_t>> ret;
  // Prepare the queue for the recursion
  std::stack<std::pair<record_t, cost_t>> stack;
  stack.push(std::make_pair(instance, cost));
  while (!stack.empty()) {
    // Pop the top element
    const auto [x, b] = stack.top();
    stack.pop();
    //
    ret.emplace_back(x, b);
    // Repopulate the queue
    for (const auto &rule : rules_) {
      if (rule.isApplicable(featureId, instance[featureId]) &&
          budget_ >= b + rule.getCost()) {
        const auto newInstanceAndCost =
            std::make_pair(rule.apply(instance), b + rule.getCost());
        // I do not want to insert duplicates based on
        bool isPresent = [&]() -> bool {
          for (const auto &i : ret) {
            if (isEqualPerturbation(i, newInstanceAndCost)) {
              return true;
            }
          }
          return false;
        }();

        if (!isPresent) {
          ret.emplace_back(newInstanceAndCost);
        }
        // TODO: check with Lucchese if we need the IF-block at line 367
        //       I don't think so, is_applicable already checks the extremes
      }
    }
  }

  return ret;
}
