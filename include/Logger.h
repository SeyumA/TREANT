//
// Created by dg on 30/10/19.
//

#ifndef TREEANT_LOGGER_H
#define TREEANT_LOGGER_H

#include <string>

class Logger {
public:
  static Logger& get_instance();

  Logger(Logger const&)          = delete;
  void operator=(Logger const&)  = delete;

  void error(const std::string&) const;

private:
  Logger() = default;
};

#endif // TREEANT_LOGGER_H
