//
// Created by dg on 08/11/19.
//

#include "Dataset.h"

#include <fstream>
#include <regex>
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
  // Refactoring ---------------------------------------------------------------
  const std::regex e("([ ]+)");
  auto cleanLine = [&e](const std::string &s) {
    std::string lineClean = std::regex_replace(s, e, " ");
    lineClean = lineClean.substr(lineClean.find_first_not_of(' '));
    lineClean = lineClean.substr(lineClean.find_last_not_of(' ') + 1);
    return lineClean;
  };
  // Reading the first line
  std::optional<std::size_t> labelPos = std::nullopt;
  std::string line;
  if (std::getline(ifs, line)) {
    line = cleanLine(line);
    std::istringstream is(line);
    std::string type;
    std::size_t i = 0;
    while (std::getline(is, type, ' ')) {
      if (type == "BOOL") {
        featureColumns_.emplace_back(bool_vector_t());
      } else if (type == "INT") {
        featureColumns_.emplace_back(int_vector_t());
      } else if (type == "DOUBLE") {
        featureColumns_.emplace_back(double_vector_t());
      } else if (type == "LABEL") {
        labelPos = i;
      } else {
        std::string err = "Cannot recognize type '";
        err.append(type);
        err.append("' in the first line of file ");
        err.append(featureFilePath);
        throw std::runtime_error(err);
      }
      i++;
    }
  } else {
    throw std::runtime_error("Cannot read the first line");
  }
  //
  if (!labelPos.has_value()) {
    throw std::runtime_error("Cannot find 'LABEL' in the first line (there "
                             "must be a column of labels");
  }
  //
  std::size_t numberOfRecords = 0;
  while (std::getline(ifs, line)) {
    line = cleanLine(line);
    std::istringstream is(line);
    std::string token;
    std::size_t j = 0;
    while (std::getline(is, token, ' ')) {
      if (j == labelPos) {
        labelVector_.emplace_back(std::stoi(token));
      } else {
        std::visit(
            [&token](auto &&arg) {
              using T = std::decay_t<decltype(arg)>;
              if (std::is_same_v<T, bool_vector_t>) {
                if (token == "0" || token == "false") {
                  arg.emplace_back(false);
                } else if (token == "1" || token == "true") {
                  arg.emplace_back(true);
                } else {
                  throw std::runtime_error(
                      "Bool variables must be '0' or '1' or 'false' or 'true'");
                }
              } else if constexpr (std::is_same_v<T, int_vector_t>) {
                arg.emplace_back(std::stoi(token));
              } else if constexpr (std::is_same_v<T, double_vector_t>) {
                arg.emplace_back(std::stod(token));
              } else {
                throw std::runtime_error(
                    "Invalid feature column (not handled in the visitor");
              }
            },
            featureColumns_[j]);
      }
      j++;
    }
    numberOfRecords++;
  }

  /*
   else if constexpr (std::is_same_v<T, int_vector_t>) {
                arg.emplace_back(std::stoi(token));
              } else if constexpr (std::is_same_v<T, double_vector_t>) {
                arg.emplace_back(std::stod(token));
              } else {
                throw std::runtime_error(
                    "Invalid feature column (not handled in the visitor");
              }
   */


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
Dataset::getMostFrequentLabel(const std::vector<index_t> &validIndexes) const {
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

const std::vector<feature_vector_t> &Dataset::getFeatureColumns() const {
  return featureColumns_;
}
