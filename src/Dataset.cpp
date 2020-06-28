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

Dataset::Dataset(const std::string &featureFilePath) {

  std::ifstream ifs;
  ifs.open(featureFilePath);
  if (!ifs.is_open() || !ifs.good()) {
    throw std::runtime_error("The record file stream is not open or not good");
  }
  // Reading the first line and populate the empty dataset but with the right
  // names
  std::optional<std::size_t> labelPos = std::nullopt;
  std::string line;
  char lineDelimiter = ' ';
  if (std::getline(ifs, line)) {
    std::istringstream is(line);
    std::string featureName;
    std::size_t i = 0;
    while (std::getline(is, featureName, lineDelimiter)) {
      if (!featureName.empty()) {
        if (featureName == "LABEL") {
          if (labelPos.has_value()) {
            throw std::runtime_error("Only one column can contain labels");
          } else {
            labelPos = i;
          }
        } else {
          featureNames_.push_back(featureName);
        }
        i++;
      }
    }
  } else {
    throw std::runtime_error("Cannot read the first line");
  }
  //
  if (!labelPos.has_value()) {
    throw std::runtime_error("Cannot find 'LABEL' in the first line (there "
                             "must be a column of labels");
  }
  // Check for duplicates
  if (featureNames_.size() !=
      std::set<std::string>(featureNames_.begin(), featureNames_.end())
          .size()) {
    throw std::runtime_error(
        "Label names are not unique, there are duplicates");
  }
  //
  // Populate the dataset
  // - resize the columns vector
  featureColumns_.resize(featureNames_.size());
  featureIsNumeric_.resize(featureNames_.size());
  std::size_t columnLimit = featureColumns_.size() + 1;
  // - initialize the counter of lines (the first line is already read
  std::size_t countLines = 1;

  feature_t mapValue = 0.0;
  if (!categoricalToDouble_.empty()) {
    std::runtime_error("categoricalToDouble map must be empty here!");
  }
  const auto getDoubleFromToken = [&mapValue, this](const std::string &token) {
    try {
      return std::make_pair(std::stod(token), true);
    } catch (std::exception &) {
      const auto [iter, isInserted] =
          this->categoricalToDouble_.insert({token, mapValue});
      if (isInserted) {
        mapValue++;
      }
      return std::make_pair(iter->second, false);
    }
  };
  //
  // Build the map of categorical entries and find out which are numerical
  if (std::getline(ifs, line)) {
    countLines++;
    std::istringstream is(line);
    std::string token;
    index_t j = 0;
    while (std::getline(is, token, lineDelimiter)) {
      if (!token.empty()) {
        if (j == labelPos) {
          labelVector_.push_back(std::stod(token));
        } else {
          const auto [valueToInsert, isNumerical] = getDoubleFromToken(token);
          featureColumns_[j].push_back(valueToInsert);
          featureIsNumeric_[j] = isNumerical;
        }
        j++;
      }
    }
    if (j != columnLimit) {
      throw std::runtime_error(
          utils::format("One missing feature at line {}", countLines));
    }
  } else {
    throw std::runtime_error("Cannot find second line after the header line");
  }

  // Build the rest of the database
  while (std::getline(ifs, line)) {
    countLines++;
    std::istringstream is(line);
    std::string token;
    std::size_t j = 0;  // index on the columns of the file
    std::size_t jj = 0; // index on the columns of the dataset.vector
    while (std::getline(is, token, lineDelimiter)) {
      if (!token.empty()) {
        // Check on the column limit
        if (j >= columnLimit) {
          throw std::runtime_error(
              utils::format("At line {} I found more data than expected ({})",
                            countLines, columnLimit));
        }
        //
        if (j == labelPos) { // Case where the we are reading the label that is
                             // a boolean
          if (token == "0") {
            labelVector_.emplace_back(0.0);
          } else if (token == "1") {
            labelVector_.emplace_back(1.0);
          } else {
            throw std::runtime_error(
                utils::format("Invalid binary label at line {}, allowed label "
                              "are '0', '1'",
                              countLines));
          }
        } else { // Case where we are reading a feature
          const auto [valueToInsert, isNumerical] = getDoubleFromToken(token);
          if (isNumerical != featureIsNumeric_[j]) {
            const std::string type =
                featureIsNumeric_[j] ? "numerical" : "categorical";
            throw std::runtime_error(utils::format(
                "The feature '{}' at line {} is supposed to be {} but it s not",
                token, type));
          }
          featureColumns_[jj].push_back(valueToInsert);
          jj++;
        }
        j++; // always update this index if a token is not empty
      }
    }
    if (j != columnLimit) {
      throw std::runtime_error(
          utils::format("One missing feature at line {}", countLines));
    }
  }
  // Close the file stream
  ifs.close();
  // Check on the size of the columns
  for (const auto &c : featureColumns_) {
    if (c.size() != labelVector_.size()) {
      throw std::runtime_error(
          utils::format("ERROR: All the feature columns must have the same "
                        "size of the label vector. Column has a size of {} and "
                        "labels are {}",
                        c.size(), labelVector_.size()));
    }
  }
  //
  if (countLines == 1 || featureColumns_[0].empty() || labelVector_.empty()) {
    throw std::runtime_error("Cannot build an empty dataset (invalid file)");
  }

  for (const auto &[s, d] : categoricalToDouble_) {
    const auto iterIsInserted = categoricalToDoubleReversed_.insert({d, s});
    if (!iterIsInserted.second) {
      throw std::runtime_error("Duplicate value in categoricalToDouble map, "
                               "the map must be a bijective function");
    }
  }
}

Dataset::Dataset(const double *X,
                 const unsigned rows,
                 const unsigned cols,
                 const double *y,
                 const std::string &isNumerical,
                 const std::string &notNumericalEntries,
                 const std::string &columnNames){
  if (!X || !y) {
    throw std::runtime_error("Invalid argument: X, y, isNumerical must not be NULL");
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
  // Assuming X entries are stored with C order
  featureColumns_.resize(cols);
  // Copy the dataset
  std::size_t counter = 0;
  for (std::size_t i{0}; i < rows; ++i) {
    for (auto& col : featureColumns_) {
      col.push_back(X[counter]);
      counter++;
    }
  }
  if (counter != rows * cols) {
    throw std::runtime_error("Counter mismatch");
  }
  // Update is numerical
  {
    std::cout << "isNumerical: " << isNumerical << std::endl;
    std::istringstream iss(isNumerical);
    std::string token;
    char delimiter = ',';
    while (std::getline(iss, token, delimiter)) {
      featureIsNumeric_.push_back(token == "True" ? true : false);
    }
    std::cout << "isNumerical stored" << std::endl;
    for (const auto& isNum : featureIsNumeric_) {
      std::cout << isNum << ' ';
    }
    std::cout << std::endl;
  }
  // Update the labels
  labelVector_.resize(rows);
  for (std::size_t i{0}; i < rows; ++i) {
    labelVector_[i] = y[i];
  }
  // Assign feature names
  {
    std::cout << "columnNames: " << columnNames << std::endl;
    std::istringstream iss(columnNames);
    std::string token;
    char delimiter = ',';
    while (std::getline(iss, token, delimiter)) {
      featureNames_.push_back(token);
    }
    const std::set<std::string> mySet(featureNames_.begin(), featureNames_.end());
    if (mySet.size() != featureNames_.size()) {
      throw std::runtime_error("Column names must be unique");
    }
  }
  if (featureColumns_.size() != featureNames_.size()) {
    throw std::runtime_error("Column names must be equal to the number of columns");
  }
  // Final controls
  for (const auto& col : featureColumns_) {
    if (!(col.size() == labelVector_.size() && col.size() == rows)) {
      throw std::runtime_error("Columns size mismatch");
    }
  }
  if (!(cols == featureNames_.size() && featureColumns_.size() && featureIsNumeric_.size())) {
    throw std::runtime_error("Columns size mismatch between featureNames_, featureColumns_ and featureIsNumeric_");
  }
}

const std::vector<label_t> &Dataset::getLabels() const { return labelVector_; }

const std::vector<std::vector<feature_t>> &Dataset::getFeatureColumns() const {
  return featureColumns_;
}

const std::vector<feature_t> &Dataset::getFeatureColumn(index_t i) const {
  return featureColumns_[i];
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
  for (std::size_t i = 0; i < ds.size(); i++) {
    os << std::setw(indexWidth) << std::left << i;
    for (index_t j = 0; j < ds.featureColumns_.size(); j++) {
      const auto &col = ds.featureColumns_[j];
      if (ds.featureIsNumeric_[j]) {
        os << std::fixed << std::setw(doubleWidth) << std::left << col[i];
      } else {
        os << std::fixed << std::setw(doubleWidth) << std::left
           << ds.categoricalToDoubleReversed_.at(col[i]);
      }
    }
    os << "|\t" << ds.labelVector_[i] << std::endl;
  }

  os << "\n\nfeatureIsNumeric:" << std::endl;
  for (const auto& s : ds.featureIsNumeric_) {
    os << s << ' ';
  }
  //
  os << "\n\ncategoricalToDouble:" << std::endl;
  for (const auto& [k, v] : ds.categoricalToDouble_) {
    os << '\t' << k << ": " << v << std::endl;
  }
  //
  os << "\n\ncategoricalToDoubleReversed:" << std::endl;
  for (const auto& [k, v] : ds.categoricalToDoubleReversed_) {
    os << '\t' << k << ": " << v << std::endl;
  }
  //
  os << "\n\nLabels:" << std::endl;
  for (const auto& l : ds.labelVector_) {
    os << '\t' << l << std::endl;
  }
  return os;
}

prediction_t Dataset::getDefaultPrediction() const {
  prediction_t prediction = 0.0;
  for (const auto &l : labelVector_) {
    prediction += l;
  }
  return prediction / (prediction_t)labelVector_.size();
}

record_t Dataset::getRecord(index_t i) const {
  record_t ret(featureColumns_.size());
  auto it = ret.begin();
  for (const auto &col : featureColumns_) {
    *it = col[i];
    it++;
  }
  return ret;
}
