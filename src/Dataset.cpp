//
// Created by dg on 08/11/19.
//

#include "Dataset.h"

#include <features/BoolFeatureVector.h>
#include <features/DoubleFeatureVector.h>
#include <features/IntFeatureVector.h>
#include <fstream>
#include <numeric>
#include <sstream>
#include <utility>

#include "utils.h"

Dataset::Dataset(const std::string &featureFilePath,
                 const std::string &labelFilePath) {
  std::ifstream ifs;
  ifs.open(featureFilePath);
  if (!ifs.is_open() || !ifs.good()) {
    throw std::runtime_error("The record file stream is not open or not good");
  }
  //
  // Read the header with the types. For each type we should be able to store
  // the proper column type (BoolColumn, Int32Column, ...)
  std::string line;
  if (std::getline(ifs, line)) {
    std::istringstream is(line);
    std::string type;
    while (std::getline(is, type, ' ')) {
      if (type == "BOOL") {
        headers_.push_back(FeatureTypes::BOOL);
      } else if (type == "INT") {
        headers_.push_back(FeatureTypes::INT);
      } else if (type == "DOUBLE") {
        headers_.push_back(FeatureTypes::DOUBLE);
      } else {
        throw std::runtime_error("Cannot recognize type '" + type + "'");
      }
    }
  } else {
    throw std::runtime_error("Cannot read the first line");
  }
  //
  // Build the records
  // Matrix of strings useful as support matrix in order to build columns
  std::vector<std::vector<std::string>> columnsAsStrings(headers_.size());
  //
  while (std::getline(ifs, line)) {
    std::istringstream is(line);
    std::string token;
    for (std::size_t j = 0; j < headers_.size(); j++) {
      if (!std::getline(is, token, ' ')) {
        throw std::runtime_error("Cannot find the current token");
      }
      columnsAsStrings[j].push_back(token);
    }
  }
  //
  if (columnsAsStrings[0].empty()) {
    throw std::runtime_error("No record find");
  }
  const std::size_t numberOfRecords = columnsAsStrings[0].size();
  // Translate the matrix of strings to the proper type
  for (std::size_t j = 0; j < headers_.size(); j++) {
    const auto type = headers_[j];
    switch (type) {
    case FeatureTypes::BOOL: {
      std::vector<bool_feature_t> column(numberOfRecords);
      for (std::size_t i = 0; i < columnsAsStrings[j].size(); i++) {
        if (columnsAsStrings[j][i] == "0" ||
            columnsAsStrings[j][i] == "false") {
          column[i] = false;
        } else if (columnsAsStrings[j][i] == "1" ||
                   columnsAsStrings[j][i] == "true") {
          column[i] = true;
        } else {
          throw std::invalid_argument(
              "Boolean feature can be only '1', '0', 'true', 'false'");
        }
      }
      featureColumns_.emplace_back(column);
      break;
    }
    case FeatureTypes::INT: {
      std::vector<int_feature_t> column(numberOfRecords);
      for (std::size_t i = 0; i < columnsAsStrings[j].size(); i++) {
        column[i] = std::stoi(columnsAsStrings[j][i]);
      }
      featureColumns_.emplace_back(column);
      break;
    }
    case FeatureTypes::DOUBLE: {
      std::vector<double_feature_t> column(numberOfRecords);
      for (std::size_t i = 0; i < columnsAsStrings[j].size(); i++) {
        column[i] = std::stod(columnsAsStrings[j][i]);
      }
      featureColumns_.emplace_back(column);
      break;
    }
    default:
      throw std::runtime_error("Not managed FeatureType");
    }
  }
  // Close the file stream
  ifs.close();
  //
  // Update the labels supposing that a label is of type int
  ifs.open(labelFilePath);
  if (!ifs.is_open() || !ifs.good()) {
    throw std::runtime_error("The label file stream is not open or not good");
  }
  labelVector_.resize(numberOfRecords);
  std::size_t labelNumber = 0;
  while (std::getline(ifs, line)) {
    labelVector_.push_back(std::stoi(line));
    labelNumber++;
  }
  if (labelNumber != numberOfRecords) {
    std::runtime_error("The label number does not match the number of records");
  }
  // Close the file stream
  ifs.close();
}

std::pair<label_t, frequency_t>
Dataset::getMostFrequentLabel(const std::vector<index_t>& validIndexes) const {
  std::map<label_t, frequency_t> labelToFrequency;
  // WARNING: Always work on the subset!
  for (const auto &index : validIndexes) {
    const auto &label = labelVector_[index];
    if (labelToFrequency.find(label) == labelToFrequency.end()) {
      labelToFrequency[label] = 1;
    } else {
      labelToFrequency[label] += 1;
    }
  }
  auto mostFrequentLabel = labelToFrequency.begin()->first;
  auto freq = labelToFrequency.begin()->second;
  for (const auto &[la, fr] : labelToFrequency) {
    if (fr > freq) {
      mostFrequentLabel = la;
      freq = fr;
    }
  }
  return std::make_pair(mostFrequentLabel, freq);
}

const std::vector<std::int32_t> &Dataset::getLabels() const {
  return labelVector_;
}

const std::vector<feature_vector_t> &
Dataset::getFeatureColumns() const {
  return featureColumns_;
}
