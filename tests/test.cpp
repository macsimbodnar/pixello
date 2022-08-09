#include "pixello.hpp"
#include <iostream>
#include <vector>

pixello::texture media1;
pixello::texture media2;

class pixel : public pixello {
public:
  pixel()
      : pixello({5, 5, 800, 800, "Test pixello", 60,
                 "assets/font/PressStart2P.ttf"}) {}

private:
  void log(const std::string &msg) override { std::cout << msg << std::endl; }

  void on_init() override {
    media1 = load_texture("assets/sample_640x426.bmp");
    media2 = load_texture("assets/Chess_klt60.png");
  }

  void on_update() override {
    clear({0xFF000000});

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

    // View port, Images
    set_current_viewport(0, 0, 320, 213);
    draw_texture(media1, 10, 10, 300, 193);
    draw_texture(media2, 0, 0);

    // Text
    set_current_viewport(800 - 320, 800 - 213, 320, 213);
    p.n = 0xFF0000FF;
    text text1 = create_text("Hello world!!!!", p);
    draw_text(text1, 20, 100, text1.width() / 2, text1.height() / 2);
  }
};

int main() {
  pixel p;

  p.run();

  return 0;
}