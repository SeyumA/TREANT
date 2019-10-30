//
// Created by dg on 30/10/19.
//

#include <iostream>

#include "Logger.h"

Logger& Logger::get_instance() {
    // This is called only once at the first call of get_instance()
    static Logger instance;
    return instance;
}

void Logger::error(const std::string& s) const {
  std::cout << "ERR: " + s << std::endl;
  // TODO: the string can also be inserted in a log file
}