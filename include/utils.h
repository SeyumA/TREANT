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

namespace utils {

std::map<std::string, std::string> get_options_map(const std::string& args);

std::vector<std::string> splitString(const std::string& s, char delimiter = ' ');

std::pair<INode *, std::size_t> buildRecursively(
    const Dataset &dataset, const std::vector<index_t> &validIndexes,
    const std::size_t &maxHeight, const std::size_t &callerDepth);
}

#endif // TREEANT_UTILS_H
