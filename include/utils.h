//
// Created by dg on 31/10/19.
//

#ifndef TREEANT_UTILS_H
#define TREEANT_UTILS_H

#include <string>
#include <map>
#include "types.h"

class INode;
class Dataset;
class IFeatureVectorVisitor;

namespace utils {

std::map<std::string, std::string> get_options_map(const std::string& args);

std::vector<std::string> splitString(const std::string& s, char delimiter = ' ');

/**
 * Recursively builds a decision tree
 * @param dataset the original dataset
 * @param subset is a vector of indexes that picks a subset of the dataset
 * @param splitter the ISplitter used to find the best split for this tree
 * @param maxDepth the maximum depth of the resulting tree
 * @param callerDepth the depth of the caller node
 * @param visitor is the IFeatureVisitor used to visit the nodes and build the tree
 * @return a pair containing the tree root and the height of the tree rooted by
 * the first output
 */
std::pair<INode *, std::size_t> buildRecursively(
    const Dataset &dataset, const std::vector<index_t> &validIndexes,
    const std::size_t &maxHeight, const std::size_t &callerDepth,
    IFeatureVectorVisitor* visitor);
}

#endif // TREEANT_UTILS_H
