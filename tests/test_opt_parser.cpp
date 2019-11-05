//
// Created by dg on 04/11/19.
//

// STD library
#include <string>
#include <cassert>
// project headers
#include "OptParser.h"


int main() {
  // Simulating the argument string
  std::string args_one = "pippo -B --i 99 --s pluto --d   -8 --missing-i -- --r 7";
  // Create the OptParser
  OptParser opt_parser(args_one);
  //
  const auto my_string_opt = opt_parser.get_string("s");
  assert(my_string_opt.has_value() && *my_string_opt == "pluto");
  //
  const auto my_integer_opt = opt_parser.get_int("i");
  assert(my_integer_opt.has_value() && *my_integer_opt == 99);
  //
  const auto my_fake_flag_opt = opt_parser.get_int("B");
  assert(!my_fake_flag_opt.has_value());
  //
  const auto my_d_flag_opt = opt_parser.get_int("d");
  assert(my_d_flag_opt.has_value() && my_d_flag_opt.value() == -8);
  //
  const auto my_missing_flag_opt = opt_parser.get_int("missing-i");
  assert(!my_missing_flag_opt.has_value());
  //
  const auto my_empty_flag_opt = opt_parser.get_int("");
  assert(!my_empty_flag_opt.has_value());
  //
  return 0;
}