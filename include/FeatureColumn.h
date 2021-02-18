//
// Created by dg on 19/02/20.
//

#ifndef TREEANT_FEATURECOLUMN_H
#define TREEANT_FEATURECOLUMN_H

#include <string>
#include <set>

#include "types.h"

class FeatureColumn {

    using fp_feature_t = double;
    using ct_feature_t = std::string;
    using generic_feature_t = std::variant<fp_feature_t, ct_feature_t >;

public:
  void addElement(const std::string&);
  [[nodiscard]] bool isNumerical() const;
  //
  [[nodiscard]] bool empty() const;
  [[nodiscard]] std::size_t size() const;

private:
  std::vector<fp_feature_t> fp_feature_vec_;
  std::vector<ct_feature_t> ct_feature_vec_;
};

#endif // TREEANT_FEATURECOLUMN_H
