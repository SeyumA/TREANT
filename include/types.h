//
// Created by dg on 06/11/19.
//

#ifndef TREEANT_TYPES_H
#define TREEANT_TYPES_H

#include <cstdint>
#include <variant>
#include <vector>

typedef std::int32_t label_t;
// Insert in the variant the types allowed (be careful: the order matters)
typedef std::variant<bool, std::int32_t, double> feature_t;
typedef std::vector<feature_t> record_t;
typedef std::vector<std::pair<record_t, label_t>> dataset_t;

#endif // TREEANT_TYPES_H
