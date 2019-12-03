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
  // types
  std::optional<std::size_t> labelPos = std::nullopt;
  std::string line;
  if (std::getline(ifs, line)) {
    std::istringstream is(line);
    std::string type;
    std::size_t i = 0;
    while (std::getline(is, type, ' ')) {
      if (!type.empty()) {
        if (type == "BOOL") {
          featureColumns_.emplace_back(bool_vector_t());
        } else if (type == "INT") {
          featureColumns_.emplace_back(int_vector_t());
        } else if (type == "DOUBLE") {
          featureColumns_.emplace_back(double_vector_t());
        } else if (type == "LABEL") {
          if (labelPos.has_value()) {
            throw std::runtime_error("Only one column can contain labels");
          } else {
            labelPos = i;
          }
        } else {
          std::stringstream ss;
          ss << "Cannot recognize type '" << type
             << "' in the first line of file " << featureFilePath;
          throw std::runtime_error(ss.str());
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
  if (featureColumns_.empty()) {
    throw std::runtime_error("There are no features in this dataset");
  }
  // Populate the dataset
  std::size_t numberOfRecords = 0;
  while (std::getline(ifs, line)) {
    std::istringstream is(line);
    std::string token;
    std::size_t j = 0;  // index on the columns of the file
    std::size_t jj = 0; // index on the columns of the dataset.vector
    while (std::getline(is, token, ' ')) {
      if (!token.empty()) {
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
                    throw std::runtime_error("Bool variables must be '0' or "
                                             "'1' or 'false' or 'true'");
                  }
                } else if (std::is_same_v<T, int_vector_t>) {
                  arg.emplace_back(std::stoi(token));
                } else if (std::is_same_v<T, double_vector_t>) {
                  arg.emplace_back(std::stod(token));
                } else {
                  throw std::runtime_error(
                      "Invalid feature column (not handled in the visitor");
                }
              },
              featureColumns_[jj]);
          jj++;
        }
        j++; // always update this index if a token is not empty
      }
    }
    if (!line.empty()) {
      numberOfRecords++;
    } else if (!(jj == featureColumns_.size() && j == (jj + 1))) {
      std::stringstream ss;
      ss << "ERROR reading record #" << numberOfRecords << " at line '" << line
         << "' where are supposed be present " << featureColumns_.size()
         << " features and one label";
      std::runtime_error(ss.str());
    }
  }
  // Close the file stream
  ifs.close();
  //
  if (!numberOfRecords) {
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

const std::vector<std::int32_t> &Dataset::getLabels() const {
  return labelVector_;
}

const std::vector<feature_vector_t> &Dataset::getFeatureColumns() const {
  return featureColumns_;
}

std::ostream &operator<<(std::ostream &os, const Dataset &ds) {
  static const int indexWidth = 7;
  static const int boolWidth = 7;
  static const int intWidth = 8;
  static const int doubleWidth = 12;

  os << std::setw(indexWidth) << std::left << "index";
  for (const auto &col : ds.featureColumns_) {
    std::visit(
        [&](auto &&arg) {
          using T = std::decay_t<decltype(arg)>;
          if (std::is_same_v<T, bool_vector_t>) {
            os << std::setw(boolWidth) << std::left << "BOOL";
          } else if (std::is_same_v<T, int_vector_t>) {
            os << std::setw(intWidth) << std::left << "INT";
          } else if (std::is_same_v<T, double_vector_t>) {
            os << std::setw(doubleWidth) << std::left << "DOUBLE";
          } else {
            throw std::runtime_error(
                "Invalid feature column (not handled in the visitor");
          }
        },
        col);
  }
  os << "|\tLABEL" << std::endl;

  for (std::size_t i = 0; i < ds.size(); i++) {
    os << std::setw(indexWidth) << std::left << i;
    for (const auto &col : ds.featureColumns_) {
      std::visit(
          [&](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if (std::is_same_v<T, bool_vector_t>) {
              if (arg[i]) {
                os << std::setw(boolWidth) << std::left << "true";
              } else {
                os << std::setw(boolWidth) << std::left << "false";
              }
            } else if (std::is_same_v<T, int_vector_t>) {
              os << std::setw(intWidth) << std::left << arg[i];
            } else if (std::is_same_v<T, double_vector_t>) {
              os << std::fixed << std::setw(doubleWidth) << std::left << arg[i];
            } else {
              throw std::runtime_error(
                  "Invalid feature column (not handled in the visitor");
            }
          },
          col);
    }
    os << "|\t" << ds.labelVector_[i] << std::endl;
  }
  return os;
}
