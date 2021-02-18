//
// Created by dg on 30/10/19.
//
#include <regex>

#include "OptParser.h"
#include "Logger.h"
#include "utils.h"

OptParser::OptParser(const std::string& args_c) {
  opt_map_ = utils::get_options_map(args_c);
}

void OptParser::reset(const std::string &args) {
  opt_map_.clear();
  opt_map_ = utils::get_options_map(args);
}

std::optional<std::int32_t> OptParser::get_int(const std::string &key) {
  if (opt_map_.find(key) != opt_map_.end()) {
    try {
      return std::stoi(opt_map_[key]);
    } catch (std::invalid_argument&) {
      Logger::get_instance().error("Cannot convert '" +
                                       opt_map_[key] + "' to integer");
      return {};
    }
  } else {
    return {};
  }
}

std::optional<std::string> OptParser::get_string(const std::string &key) {
  if (opt_map_.find(key) != opt_map_.end()) {
    return opt_map_[key];
  } else {
    return std::nullopt;
  }
}