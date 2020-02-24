//
// Created by dg on 08/11/19.
//

#include "Dataset.h"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <utility>

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
  //
  // Populate the dataset
  // - resize the columns vector
  featureColumns_.resize(featureNames_.size());
  // - initialize the counter of lines (the first line is already read
  std::size_t countLines = 1;
  std::size_t columnLimit = featureColumns_.size() + 1;
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
          featureColumns_[jj].addElement(token);
          jj++;
        }
        j++; // always update this index if a token is not empty
      }
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

const std::vector<label_t> &Dataset::getLabels() const { return labelVector_; }

const std::vector<FeatureColumn> &Dataset::getFeatureColumns() const {
  return featureColumns_;
}

const FeatureColumn &Dataset::getFeatureColumn(index_t i) const {
  return featureColumns_[i];
}

std::string Dataset::getFeatureName(index_t i) const {
  return featureNames_[i];
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
    for (const auto &col : ds.featureColumns_) {
      if (col.isNumerical()) {
        os << std::fixed << std::setw(doubleWidth) << std::left
           << col.getFpFeature(i);
      } else {
        os << std::fixed << std::setw(doubleWidth) << std::left
           << col.getCtFeature(i);
      }
    }
    os << "|\t" << ds.labelVector_[i] << std::endl;
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
