#include <DecisionTree.h>
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

  {
    INode* root = new BinIntNode(0,0,22, new Leaf(1,0), new Leaf(1,1));
    record_t r = {23, "pippo", "pluto"};
    auto label_prediction = root->predict(r);
    std::cout << "label_prediction is = " << label_prediction << std::endl;
  }

  std::cout << "Just it" << std::endl;
  std::cout << "End of program" << std::endl;
  return 0;

}
