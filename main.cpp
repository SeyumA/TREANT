#include <iostream>
#include <regex>
#include <string>

#include "Logger.h"
#include "OptParser.h"


int main() {
  std::cout << "Start of the program" << std::endl;

  OptParser opt_parser("pippo -B -i a");

  auto my_integer_opt = opt_parser.get_int("i");
  if (my_integer_opt.has_value()){
    std::cout << "my_integer is " << *my_integer_opt << std::endl;
  } else {
    std::cout << "my_integer is invalid" << std::endl;
  }


  std::cout << "End of program" << std::endl;
  return 0;

}
