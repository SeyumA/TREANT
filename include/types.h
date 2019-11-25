//
// Created by dg on 06/11/19.
//

#ifndef TREEANT_TYPES_H
#define TREEANT_TYPES_H

#include <cstdint> // needed for std::int32_t
#include <variant> // needed for std::variant
#include <vector>  // needed for std::vector

using frequency_t = std::size_t;
using label_t = std::int32_t;
using partition_t = std::vector<std::size_t>;
using partitions_t = std::vector<std::vector<std::size_t>>;
using index_t = std::size_t;
using indexes_t = std::vector<index_t>;

using bool_feature_t = bool;
using int_feature_t = std::int32_t;
using double_feature_t = double;
using feature_t = std::variant<bool_feature_t, int_feature_t, double_feature_t>;

// The record_t must contain all the possible kind of feature types.
using record_t = std::vector<feature_t>;

// There must be a one to one correspondence between feature types
// and feature vectors.
using bool_vector_t = std::vector<bool_feature_t>;
using int_vector_t = std::vector<int_feature_t>;
using double_vector_t = std::vector<double_feature_t>;

// The feature_vector_t must contain all the possible kind of feature vectors.
using feature_vector_t =
    std::variant<bool_vector_t, int_vector_t, double_vector_t>;

#endif // TREEANT_TYPES_H
