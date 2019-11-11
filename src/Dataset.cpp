//
// Created by dg on 08/11/19.
//

#include "Dataset.h"

#include <fstream>
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
      } else if (type == "INT_32") {
        headers_.push_back(FeatureTypes::INT32);
      } else if (type == "DOUBLE") {
        headers_.push_back(FeatureTypes::DOUBLE);
      } else {
        throw std::runtime_error("Cannot recognize type '" + type + "'");
      }
    }

    // Working code ----------------------------------
    //    const auto types = utils::splitString(line);
//    for (const auto &type : types) {
//      if (type == "BOOL") {
//        headers_.push_back(FeatureTypes::BOOL);
//      } else if (type == "INT_32") {
//        headers_.push_back(FeatureTypes::INT32);
//      } else if (type == "DOUBLE") {
//        headers_.push_back(FeatureTypes::DOUBLE);
//      } else {
//        throw std::runtime_error("Cannot recognize type '" + type + "'");
//      }
//    }
  } else {
    throw std::runtime_error("Cannot read the first line");
  }
  //
  // Build the records
  bool supp_bool;
  std::int32_t supp_int;
  double supp_double;
  //
  while (std::getline(ifs, line)) {
    std::istringstream is(line);
    record_t currRecord;
    for (const auto type : headers_) {
      switch (type) {
      case FeatureTypes::BOOL:
        // Be careful! Bool type must be 'true' or 'false' in the file
        is >> std::boolalpha >> supp_bool;
        currRecord.push_back(supp_bool);
        break;
      case FeatureTypes::INT32:
        is >> supp_int;
        currRecord.push_back(supp_int);
        break;
      case FeatureTypes::DOUBLE:
        is >> supp_double;
        currRecord.push_back(supp_double);
        break;
      default:
        throw std::runtime_error("Not managed FeatureType");
      }
    }
    dataset_.push_back(std::make_pair(currRecord, 0));
  }
  // Close the file stream
  ifs.close();
  //
  // Update the labels supposing that a label is of type int
  ifs.open(labelFilePath);
  if (!ifs.is_open() || !ifs.good()) {
    throw std::runtime_error("The label file stream is not open or not good");
  }
  std::size_t recordNumber = 0;
  while (std::getline(ifs, line)) {
    dataset_[recordNumber].second = std::stoi(line);
    recordNumber++;
  }
  if (recordNumber != dataset_.size()) {
    std::runtime_error("The label number does not match the number of records");
  }
  // Close the file stream
  ifs.close();
  //
//  // Initialize valid indexes
//  validIndexes_.resize(recordNumber);
//  for (std::size_t i = 0; i < validIndexes_.size(); i++) {
//    validIndexes_[i] = i;
//  }
}

Dataset::Dataset(std::vector<FeatureTypes> headers, dataset_t dataset)
    : headers_(headers), dataset_(dataset) {}
