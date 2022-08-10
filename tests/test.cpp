#include <iostream>
#include <vector>
#include "pixello.hpp"

texture_t media1;
texture_t media2;

class pixel : public pixello
{
public:
  pixel()
      : pixello({5, 5, 800, 800, "Test pixello", 60,
                 "assets/font/PressStart2P.ttf", 10})
  {}

private:
  void log(const std::string& msg) override { std::cout << msg << std::endl; }

  void on_init() override
  {
    media1 = load_image("assets/sample_640x426.bmp");
    media2 = load_image("assets/Chess_klt60.png");
  }

  void on_update() override
  {
    clear_screen({0xFF000000});

    for (uint32_t i = 0; i < 1000; i++) {
      const uint32_t x = (rand() % (width_in_pixels() - 2)) + 1;
      const uint32_t y = (rand() % (height_in_pixels() - 2)) + 1;

      pixel_t p;
      p.r = rand() % 255;
      p.g = rand() % 255;
      p.b = rand() % 255;
      p.a = 255;

      draw_pixel(x, y, p);
    }

    pixel_t p;
    p.a = 255;

    p.r = 255;
    p.g = 0;
    p.b = 0;
    draw_pixel(0, 0, p);  // top left red

    p.r = 0;
    p.g = 255;
    p.b = 0;
    draw_pixel(width_in_pixels() - 1, 0, p);  // top tight green

    p.r = 0;
    p.g = 0;
    p.b = 255;
    draw_pixel(width_in_pixels() - 1, height_in_pixels() - 1,
               p);  // bottom right blu

    p.r = 255;
    p.g = 0;
    p.b = 255;
    draw_pixel(0, height_in_pixels() - 1, p);  // bottom left purple

    // Draw texture with transparency
    draw_texture(media2, (800 / 2) - (media2.w / 2),
                 (800 / 2) - (media2.h / 2));

    // View port, Images
    set_current_viewport(0, 0, 320, 213, {0x5500FF00});
    draw_texture(media1, 10, 10, 300, 193);

    // Text
    set_current_viewport(800 - 320, 800 - 213, 320, 213);
    p.n = 0xFF0000FF;
    texture_t text1 = create_text("Hello world!!!!", p);
    draw_texture(text1, 20, 100);
  }
};

int main()
{
  pixel p;

  if (p.run()) { return 0; }

  return 1;
}