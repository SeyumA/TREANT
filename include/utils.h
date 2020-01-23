//
// Created by dg on 31/10/19.
//

#ifndef TREEANT_UTILS_H
#define TREEANT_UTILS_H

#include "types.h"
#include <map>
#include <sstream>
#include <string>

class Node;
class Dataset;
class IFeatureVectorVisitor;

namespace utils {

std::map<std::string, std::string> get_options_map(const std::string &args);

/**
 * Recursively builds a decision tree
 * @param dataset the original dataset
 * @param subset is a vector of indexes that picks a subset of the dataset
 * @param splitter the ISplitter used to find the best split for this tree
 * @param maxDepth the maximum depth of the resulting tree
 * @param currDepth the depth of the current node
 * @param visitor is the IFeatureVisitor used to visit the nodes and build the
 * tree
 * @return a pair containing the tree root and the height of the tree rooted by
 * the first output
 */
std::pair<Node *, std::size_t>
buildRecursively(const Dataset &dataset, const std::size_t &maxHeight,
                 const std::size_t &currDepth, IFeatureVectorVisitor *visitor
                 //, attacker,
                 // cost,
                 // nodePrediction,
                 // featureBlackList,
                 // constraints
                 // ...
                 );

template <typename S1, typename S2> std::string concatenate(S1 s1, S2 s2) {
  std::stringstream ss;
  ss << std::fixed << s1 << s2;
  return ss.str();
}

template <typename First, typename... Types>
std::string format(std::string firstArg, First head, Types... args) {
  // NOTE: the check on {} can be done at compile time
  // (see
  // https://akrzemi1.wordpress.com/2011/05/11/parsing-strings-at-compile-time-part-i/)
  const auto pos = firstArg.find("{}");
  if constexpr (sizeof...(args) != 0) {
    if (pos == std::string::npos) {
      throw std::runtime_error("You are lacking '{}' in format()");
    }
    auto dummy = concatenate(firstArg.substr(0, pos), head);
    return dummy + format(firstArg.substr(pos + 2), args...);
  } else {
    const auto trailPart = firstArg.substr(pos + 2);
    if (trailPart.find("{}") != std::string::npos) {
      throw std::runtime_error("Too many '{}' in format()");
    }
    return concatenate(firstArg.substr(0, pos), head) + trailPart;
  }
}

} // namespace utils

#endif // TREEANT_UTILS_H
