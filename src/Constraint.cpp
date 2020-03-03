//
// Created by dg on 22/02/20.
//

#include "Constraint.h"
#include "Attacker.h"

Constraint::Constraint(std::pair<record_t, cost_t> x, const label_t &y,
                       const cost_t &cost, const bool &inequality,
                       const gain_t &bound)
    : x_(std::move(x)), y_(y), cost_(cost), inequality_(inequality),
      bound_(bound) {}

std::optional<Constraint> Constraint::propagateLeft(const Attacker &attacker,
                                                    index_t featureId,
                                                    feature_t featureValue,
                                                    bool isNumerical) const {

  const auto allAttacks = attacker.attack(x_, featureId, cost_);
  std::vector<std::pair<record_t, cost_t>> attacks;
  if (isNumerical) {
    for (const auto &atk : allAttacks) {
      if (atk.first[featureId] <= featureValue) {
        attacks.push_back(atk);
      }
    }
  } else {
    for (const auto &atk : allAttacks) {
      if (atk.first[featureId] == featureValue) {
        attacks.push_back(atk);
      }
    }
  }
  // Base case
  if (attacks.empty()) {
    return std::nullopt;
  }
  //
  return std::optional(
      Constraint(x_, y_, calculateMinCost(attacks), inequality_, bound_));
}

std::optional<Constraint> Constraint::propagateRight(const Attacker &attacker,
                                                     index_t featureId,
                                                     feature_t featureValue,
                                                     bool isNumerical) const {

  const auto allAttacks = attacker.attack(x_, featureId, cost_);
  std::vector<std::pair<record_t, cost_t>> attacks;
  if (isNumerical) {
    for (const auto &atk : allAttacks) {
      if (atk.first[featureId] > featureValue) {
        attacks.push_back(atk);
      }
    }
  } else {
    for (const auto &atk : allAttacks) {
      if (atk.first[featureId] != featureValue) {
        attacks.push_back(atk);
      }
    }
  }
  // Base case
  if (attacks.empty()) {
    return std::nullopt;
  }
  //
  return std::optional(
      Constraint(x_, y_, calculateMinCost(attacks), inequality_, bound_));
}

cost_t Constraint::calculateMinCost(
    const std::vector<std::pair<record_t, cost_t>> &attacks) const {
  if (attacks.empty()) {
    throw std::runtime_error(
        "Cannot calculate the minimum cost of an empty set");
  }
  // Calculate minimum cost among the attacks
  cost_t minCost = attacks[0].second;
  for (const auto &atk : attacks) {
    if (atk.second < minCost) {
      minCost = atk.second;
    }
  }
  return minCost;
}

void Constraint::setType(char type) { type_ = type; }
