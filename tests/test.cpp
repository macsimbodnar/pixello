#include "pixello.hpp"
#include <iostream>
#include <vector>

class pixel : public pixello {
public:
  pixel() : pixello("Pixello Test", 1000, 500, 100, 100) {}

private:
  void log(const std::string &msg) { std::cout << msg << std::endl; }
  void on_update(std::vector<uint8_t> &pixels) {
    for (uint32_t i = 0; i < 1000; i++) {
      const uint32_t x = rand() % w;
      const uint32_t y = rand() % h;

      const uint32_t offset = (w * 4 * y) + x * 4;
      pixels[offset + 0] = rand() % 256; // b
      pixels[offset + 1] = rand() % 256; // g
      pixels[offset + 2] = rand() % 256; // r
      pixels[offset + 3] = 255;          // a
    }
  }
};

int main() {
  pixel p;

  p.run();

  return 0;
}