//
// Created by dg on 30/10/19.
//
#include <regex>

#include "OptParser.h"
#include "Logger.h"

OptParser::OptParser(const std::string& args_c) {

  static const std::string endings = "\t\n\v\f\r ";
  // finds one or more spaces
  static const std::regex space_r("[ ]{1,}");
  // matches with a valid flag, e.g. "-name" or "-d"
  static const std::regex option_r("(-[a-zA-z]{1,})");
  // matches with a valid value for a flag, e.g. "pippo" or "3" or "p3r"
  static const std::regex value_r("^[a-zA-z0-9]+$");

  std::string args(args_c);
  // Right trim
  args.erase(0, args.find_first_not_of(endings));
  // Left trim
  args.erase(args.find_last_not_of(endings) + 1);
  // Find the flags and the corresponding values
  std::smatch match;
  while (regex_search(args, match, space_r)) {
    const auto token = args.substr(0, match.position(0));
    // If is an option then find its corresponding value
    if (std::regex_match(token, option_r)) {
      auto curr_option = token.substr(1);
      args = match.suffix().str();
      if (regex_search(args, match, value_r)){
        // This is the last flag
        opt_map.insert(std::pair<std::string, std::string>(curr_option, args));
      } else if (regex_search(args, match, space_r)){
        const auto value = args.substr(0, match.position(0));
        opt_map.insert(std::pair<std::string, std::string>(curr_option, value));
      } else {
        Logger::get_instance().error("Cannot find a value for '" + token +
          "' flag");
      };
    }
    // suffix to find the rest of the string.
    args = match.suffix().str();
  }
}

std::optional<std::int32_t> OptParser::get_int(const std::string &key) {
  if (opt_map.find(key) != opt_map.end()) {
    try {
      return std::stoi(opt_map[key]);
    } catch (std::invalid_argument&) {
      Logger::get_instance().error("Cannot convert '" +
                                   opt_map[key] + "' to integer");
      return {};
    }
  } else {
    return {};
  }
}

std::optional<std::string> OptParser::get_string(const std::string &key) {
  if (opt_map.find(key) != opt_map.end()) {
    return opt_map[key];
  } else {
    return std::nullopt;
  }
}