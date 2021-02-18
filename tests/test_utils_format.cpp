#include <iostream>
#include "utils.h"

int main() {

  std::cout << utils::format("this is a double '{}' and this is an int '{}'",
                             3.4555f, 6)
            << std::endl;

  try {
    utils::format("this is a double '{}' and this is an int '{}'",
                           3.4555f, 6, "generate error");
  } catch (std::runtime_error& ex) {
    std::cout << "ERROR using utils::format:" << std::endl;
    std::cout << ex.what();
  }

  return 0;
}