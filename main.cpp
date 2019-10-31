#include <iostream>
#include <regex>
#include <string>

#include "Logger.h"
#include "OptParser.h"


int main() {
  std::cout << "Start of the program" << std::endl;

  // Test the OptParser
  {
    OptParser opt_parser("pippo -B -i a -s pluto");
    const auto my_string_opt = opt_parser.get_int("i");
    if (my_string_opt.has_value()){
      std::cout << "my_string is " << my_string_opt.value() << std::endl;
    } else {
      std::cout << "my_integer is invalid" << std::endl;
    }

    const auto my_integer_opt = opt_parser.get_int("i");
    if (my_integer_opt.has_value()){
      std::cout << "my_integer is " << my_integer_opt.value() << std::endl;
    } else {
      std::cout << "my_integer is invalid" << std::endl;
    }
  }

  std::cout << "End of program" << std::endl;
  return 0;

}
