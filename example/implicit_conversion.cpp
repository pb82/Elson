#include <iostream>
#include "../include/Elson.hpp"

int main () {
  using namespace JSON;
  
  Object obj {
    {"id", 25}
  };
  
  int val = (int) obj["id"];
  
  std::cout << val << std::endl;
}
