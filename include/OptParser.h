//
// Created by dg on 30/10/19.
//

#ifndef TREEANT_OPTPARSER_H
#define TREEANT_OPTPARSER_H

#include <string>
#include <map>
#include <optional>

class OptParser {
public:
  explicit OptParser(const std::string& args);

  std::optional<std::int32_t> get_int(const std::string& key);

  std::optional<std::string> get_string(const std::string &key);

  void reset(const std::string& args);

private:
  std::map<std::string, std::string> opt_map_;
};

#endif // TREEANT_OPTPARSER_H
