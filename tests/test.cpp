#include "pixello.hpp"
#include <iostream>

void log(const std::string &msg) { std::cout << msg << std::endl; }

int main() {
  pixello pixel("Pixello Test", 1000, 500, 100, 100);

  pixel.set_logger(log);

  pixel.run();

  pixel.close();

  return 0;
}