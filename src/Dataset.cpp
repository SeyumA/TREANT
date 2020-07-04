//
// Created by dg on 08/11/19.
//

#include "Dataset.h"

#include <fstream>
#include <iomanip>
#include <set>
#include <sstream>
#include <utility>

// For debug
#include <iostream>

#include "utils.h"


Dataset::Dataset(const double *X, const unsigned rows, const unsigned cols,
                 const double *y, const std::string &isNumerical,
                 const std::string &notNumericalEntries,
                 const std::string &columnNames)
    : rows_(rows), cols_(cols), X_(X), y_(y) {
  if (!X || !y) {
    throw std::runtime_error(
        "Invalid argument: X, y, isNumerical must not be NULL");
  }

  if (!rows || !cols) {
    throw std::runtime_error("Invalid argument: rows and cols must be > 0");
  }
  // Populate categoricalToDouble_
  {
    std::cout << "notNumericalEntries: " << notNumericalEntries << std::endl;
    std::istringstream iss(notNumericalEntries);
    std::string token;
    char delimiter = ',';
    feature_t mapValue = 0.0;
    while (std::getline(iss, token, delimiter)) {
      if (categoricalToDouble_.insert({token, mapValue}).second) {
        mapValue++;
      } else {
        throw std::runtime_error("token '" + token +
                                 "' already present in the map");
      }
    }
  }
  // Update the categoricalToDoubleReversed_
  for (const auto &[s, d] : categoricalToDouble_) {
    if (!categoricalToDoubleReversed_.insert({d, s}).second) {
      throw std::runtime_error("Duplicate value in categoricalToDouble map, "
                               "the map must be a bijective function");
    }
  }
  // Update is numerical
  {
    //std::cout << "isNumerical: " << isNumerical << std::endl;
    std::istringstream iss(isNumerical);
    std::string token;
    char delimiter = ',';
    while (std::getline(iss, token, delimiter)) {
      featureIsNumeric_.push_back(token == "True" ? true : false);
    }
    std::cout << "isNumerical stored" << std::endl;
    for (const auto &isNum : featureIsNumeric_) {
      std::cout << isNum << ' ';
    }
    std::cout << std::endl;
  }
  // Assign feature names
  {
    //std::cout << "columnNames: " << columnNames << std::endl;
    std::istringstream iss(columnNames);
    std::string token;
    char delimiter = ',';
    while (std::getline(iss, token, delimiter)) {
      featureNames_.push_back(token);
    }
    const std::set<std::string> mySet(featureNames_.begin(),
                                      featureNames_.end());
    if (mySet.size() != featureNames_.size()) {
      throw std::runtime_error("Column names must be unique");
    }
  }
  if (cols_ != featureNames_.size()) {
    throw std::runtime_error(
        "Column names must be equal to the number of declared columns");
  }
  if (cols != featureIsNumeric_.size()) {
    throw std::runtime_error("Columns size mismatch between cols, "
                             "and featureIsNumeric_");
  }
}

std::pair<unsigned, std::vector<std::string>>
Dataset::getDatasetInfoFromFile(const std::string &datasetFilePath) {
  std::ifstream ifs;
  ifs.open(datasetFilePath);
  if (!ifs.is_open() || !ifs.good()) {
    throw std::runtime_error("The record file stream is not open or not good");
  }
  // Reading the first line and populate the list of column names
  std::string line;
  std::vector<std::string> columnNames;

  char lineDelimiter = ' ';
  if (std::getline(ifs, line)) {
    std::istringstream is(line);
    std::string featureName;
    while (std::getline(is, featureName, lineDelimiter)) {
      if (!featureName.empty()) {
        columnNames.push_back(featureName);
      }
    }
    // delete the last name that is for the labels
    columnNames =
        std::vector<std::string>(columnNames.begin(), columnNames.end() - 1);
  } else {
    throw std::runtime_error("Cannot read the first line");
  }
  // Check for duplicates in the column names
  if (columnNames.size() !=
      std::set<std::string>(columnNames.begin(), columnNames.end()).size()) {
    throw std::runtime_error(
        "Label names are not unique, there are duplicates");
  }
  //
  // count the rows
  unsigned rows = 0;
  while (std::getline(ifs, line)) {
    rows++;
  }
  return {rows, columnNames};
}

std::pair<std::vector<std::string>, std::vector<std::string>>
Dataset::fillXandYfromFile(double *X, const unsigned rows, const unsigned cols,
                           double *y, const std::string &datasetFilePath) {
  std::ifstream ifs;
  ifs.open(datasetFilePath);
  if (!ifs.is_open() || !ifs.good()) {
    throw std::runtime_error("The record file stream is not open or not good");
  }
  // Reading the first line and populate the list of column names
  std::string line;
  std::unordered_map<std::string, index_t> catToIndex;
  std::vector<std::string> columnNames;
  char lineDelimiter = ' ';
  if (!std::getline(ifs, line)) {
    throw std::runtime_error("Cannot read the first line with headers");
  }
  std::vector<std::string> isNumerical;
  index_t currMapValue = 0;
  unsigned i = 0;
  while (std::getline(ifs, line)) {
    std::istringstream is(line);
    std::string token;
    unsigned j = 0;
    while (std::getline(is, token, lineDelimiter)) {
      if (!token.empty()) {
        feature_t featureValue = 0;
        bool isNumericalFlag = true;
        try {
          featureValue = std::stod(token);
        } catch (std::exception &) {
          isNumericalFlag = false;
          if (catToIndex.find(token) == catToIndex.end()) {
            featureValue = static_cast<feature_t>(currMapValue);
            catToIndex[token] = currMapValue;
            currMapValue++;
          } else {
            featureValue = static_cast<feature_t>(catToIndex[token]);
          }
        }
        // write the feature to the X vector (column-wise order)
        if (j < cols) {
          // Update isNumerical
          if (i == 0) {
            isNumerical.push_back(isNumericalFlag ? "True" : "False");
          }
          // Update X (even if the file is read row by row, the ordering is
          // column wise in X
          X[j * rows + i] = featureValue;
        } else if (j == cols) {
          y[i] = featureValue;
        } else {
          throw std::runtime_error("the file has more cols of the one "
                                   "specified including also the labels");
        }
        // Update the column counter
        j++;
      }
    }
    i++;
  }
  // Build the list
  std::vector<std::string> notNumericalEntries(catToIndex.size());
  for (const auto &[k, v] : catToIndex) {
    notNumericalEntries[v] = k;
  }
  return std::make_pair(isNumerical, notNumericalEntries);
}

bool Dataset::empty() const {
  return !X_ || !y_ || !rows_ || !cols_;
}

std::size_t Dataset::size() const {
  return rows_;
}

bool Dataset::isFeatureNumerical(index_t j) const {
  return featureIsNumeric_[j];
}

std::string Dataset::getFeatureName(index_t i) const {
  return featureNames_[i];
}

index_t Dataset::getFeatureIndex(const std::string &featureName) const {
  for (index_t i = 0; i < featureNames_.size(); i++) {
    if (featureNames_[i] == featureName) {
      return i;
    }
  }
  throw std::runtime_error(utils::format(
      "Cannot find name '{}' in dataset feature names", featureName));
}

std::optional<feature_t>
Dataset::getCategoricalFeatureValue(const std::string &featureName) const {
  std::optional<feature_t> ret = std::nullopt;
  if (categoricalToDouble_.find(featureName) != categoricalToDouble_.end()) {
    ret = categoricalToDouble_.at(featureName);
  }
  return ret;
}

std::optional<std::string>
Dataset::getCategoricalFeatureName(const feature_t &featureValue) const {
  std::optional<std::string> ret = std::nullopt;
  if (categoricalToDoubleReversed_.find(featureValue) !=
      categoricalToDoubleReversed_.end()) {
    ret = categoricalToDoubleReversed_.at(featureValue);
  }
  return ret;
}

std::ostream &operator<<(std::ostream &os, const Dataset &ds) {
  static const int indexWidth = 7;
  static const int doubleWidth = 12;
  //
  // Print the header
  os << std::setw(indexWidth) << std::left << "index";
  for (const auto &col : ds.featureNames_) {
    os << std::setw(doubleWidth) << std::left << col;
  }
  os << "|\tLABEL" << std::endl;
  //
  // Print the features
  for (std::size_t i = 0; i < ds.rows_; i++) {
    os << std::setw(indexWidth) << std::left << i;
    for (index_t j = 0; j < ds.cols_; j++) {
      if (ds.featureIsNumeric_[j]) {
        os << std::fixed << std::setw(doubleWidth) << std::left << ds(i, j);
      } else {
        os << std::fixed << std::setw(doubleWidth) << std::left
           << ds.categoricalToDoubleReversed_.at(ds(i, j));
      }
    }
    os << "|\t" << ds(i) << std::endl;
  }

  os << "\n\nfeatureIsNumeric:" << std::endl;
  for (const auto &s : ds.featureIsNumeric_) {
    os << s << ' ';
  }
  //
  os << "\n\ncategoricalToDouble:" << std::endl;
  for (const auto &[k, v] : ds.categoricalToDouble_) {
    os << '\t' << k << ": " << v << std::endl;
  }
  //
  os << "\n\ncategoricalToDoubleReversed:" << std::endl;
  for (const auto &[k, v] : ds.categoricalToDoubleReversed_) {
    os << '\t' << k << ": " << v << std::endl;
  }
  return os;
}

prediction_t Dataset::getDefaultPrediction() const {
  prediction_t prediction = 0.0;
  for (index_t i = 0; i < rows_; i++) {
    prediction += y_[i];
  }
  return prediction / static_cast<prediction_t>(rows_);
}

record_t Dataset::getRecord(index_t i) const {
  record_t ret(cols_);
  for (index_t j = 0; j < cols_; j++) {
    ret[j] = operator()(i, j);
  }
  return ret;
}
