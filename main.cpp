#include <DecisionTree.h>
#include <iostream>
#include <regex>
#include <string>

#include "Logger.h"
#include "OptParser.h"


int main() {
  std::cout << "Start of the program" << std::endl;

  {
    INode* root = new BinIntNode(0,0,22, new Leaf(1,0), new Leaf(1,1));
    record_t r = {23, "pippo", "pluto"};
    auto label_prediction = root->predict(r);
    std::cout << "label_prediction is = " << label_prediction << std::endl;
  }

  std::cout << "End of program" << std::endl;
  return 0;

}
