#include "pixello.hpp"
#include <iostream>
#include <vector>

pixello::texture_t media;

class pixel : public pixello {
public:
  pixel() : pixello({5, 5, 800, 800, 100, 100, "Test pixello", 60}) {}

private:
  void log(const std::string &msg) override { std::cout << msg << std::endl; }

  void on_update() override {
    clear({0xFF000000});
    draw_texture(media);

    for (uint32_t i = 0; i < 1000; i++) {
      const uint32_t x = (rand() % (width_in_pixels() - 2)) + 1;
      const uint32_t y = (rand() % (height_in_pixels() - 2)) + 1;

      pixel_t p;
      p.r = rand() % 255;
      p.g = rand() % 255;
      p.b = rand() % 255;
      p.a = 255;

      draw(x, y, p);
    }

    pixel_t p;
    p.a = 255;

    p.r = 255;
    p.g = 0;
    p.b = 0;
    draw(0, 0, p); // top left red

    p.r = 0;
    p.g = 255;
    p.b = 0;
    draw(width_in_pixels() - 1, 0, p); // top tight green

    p.r = 0;
    p.g = 0;
    p.b = 255;
    draw(width_in_pixels() - 1, height_in_pixels() - 1, p); // bottom right blu

    p.r = 255;
    p.g = 0;
    p.b = 255;
    draw(0, height_in_pixels() - 1, p); // bottom left purple
  }
};

int main() {
  pixel p;

  media = p.load_texture("assets/sample_640x426.bmp");

  if (media.pointer == NULL) {
    return 1;
  }

  p.run();

  return 0;
}