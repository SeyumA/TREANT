//
// Created by dg on 31/10/19.
//

#include "utils.h"
#include "Dataset.h"
#include "Logger.h"
#include <regex>

#include "nodes/BinDoubleNode.h"
#include "nodes/BinIntNode.h"
#include "nodes/BooleanNode.h"
#include "nodes/INode.h"
#include "nodes/Leaf.h"

#include "visitors/GiniVisitor.h"

namespace utils {

std::map<std::string, std::string> get_options_map(const std::string &args_c) {

  static const std::string endings = "\t\n\v\f\r ";
  // finds one or more spaces
  static const std::regex space_r("[ ]{1,}");
  // matches with a valid flag, e.g. "--name" or "--d"
  static const std::regex option_r("(--[a-zA-z]{1,})");
  // matches with a valid value for a flag, e.g. "pippo" or "3" or "p3r"
  static const std::regex value_r("^[a-zA-z0-9]+$");
  // Make a workable copy
  std::string args(args_c);
  // Right trim
  args.erase(0, args.find_first_not_of(endings));
  // Left trim
  args.erase(args.find_last_not_of(endings) + 1);
  // Add an ending space in order to get also the last token
  args.append(" ");
  // Find all the tokens
  std::vector<std::string> tokens;
  std::smatch match;
  while (regex_search(args, match, space_r)) {
    tokens.push_back(args.substr(0, match.position(0)));
    // suffix to find the rest of the string.
    args = match.suffix().str();
  }
  // Analyse the tokens and build the dictionary
  std::map<std::string, std::string> res;
  auto it = tokens.begin();
  while (it != tokens.end()) {
    if (std::regex_match(*it, option_r)) {
      const auto curr_option = it->substr(2);
      ++it;
      if (it != tokens.end()) {
        // TODO: some checks about the next token (does it start with '-'?, ...)
        res.insert(std::pair<std::string, std::string>(curr_option, *it));
      } else {
        --it; // back of one position to match tokens.end()
      }
    }
    ++it;
  }

  return res;
}

std::vector<std::string> splitString(const std::string &s, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream iss(s);
  while (std::getline(iss, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

std::pair<INode *, std::size_t>
buildRecursively(const Dataset &dataset,
                 const std::vector<index_t> &validIndexes,
                 const std::size_t &maxHeight, const std::size_t &callerDepth,
                 IFeatureVectorVisitor *visitor) {
  //
  // Find the best split and continue building the tree
  if (callerDepth >= maxHeight) {
    throw std::runtime_error("callerDepth must be less than maxHeight");
  } else if (callerDepth + 1 == maxHeight) {
    // A leaf should be returned with the mostPopular label in the subset
    return std::make_pair(
        new Leaf(dataset.getMostFrequentLabel(validIndexes).first), 1);
  } else if (const auto [l, f] = dataset.getMostFrequentLabel(validIndexes);
             f == dataset.size()) {
    return std::make_pair(new Leaf(l), 1);
  } else {
    // Assuming that there are different labels in the current training subset
    const auto &featureColumns = dataset.getFeatureColumns();
    //
    // TODO: could be a parameter of the function buildRecursively
    //    GiniVisitor visitor(validIndexes, dataset.getLabels());
    //
    visitor->visitFeatureVectors(featureColumns);
    const auto [bestSplitter, bestPartitions] =
        visitor->getBestSplitterWithPartitions();
    //
    // This function can access the INode::children_ because is a friend of
    // INode Clean up the children (safety-first)
    for (auto &child : bestSplitter->children_) {
      delete child;
    }
    std::size_t currHeight = 0;
    for (std::size_t i = 0; i < bestSplitter->children_.size(); ++i) {
      // Build recursively the children
      IFeatureVectorVisitor *childrenVisitor = visitor->clone();
      auto [child, h] = buildRecursively(dataset, bestPartitions[i], maxHeight,
                                         callerDepth + 1, childrenVisitor);
      bestSplitter->children_[i] = child;
      currHeight = currHeight < h ? h : currHeight;
      delete childrenVisitor;
    }
    //
    return std::make_pair(bestSplitter, currHeight);
  }
}

}