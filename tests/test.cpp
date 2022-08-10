#include <iostream>
#include <vector>
#include "pixello.hpp"

texture_t media1;
texture_t media2;

int32_t media2_x;
int32_t media2_y;

std::string pos_str(mouse_t::state_t s)
{
  std::string res = "";
  switch (s) {
    case mouse_t::DOWN:
      res = "down";
      break;
    case mouse_t::UP:
      res = "up";
      break;
    case mouse_t::REST:
      res = "rest";
      break;
  }
  return res;
}

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

    media2_x = (800 / 2) - (media2.w / 2);
    media2_y = (800 / 2) - (media2.h / 2);
  }

  void on_update() override
  {
    clear_screen({0x000000FF});

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

    // Update medai2 position if mouse current state button down
    if (is_mouse_in(media2_x, media2_y, media2.w, media2.h) &&
        mouse_position().left_new_state == mouse_t::DOWN) {
      media2_x = mouse_position().x;
      media2_y = mouse_position().y;
    }

    // Draw texture with transparency
    draw_texture(media2, media2_x, media2_y);

    // View port, Images
    set_current_viewport(0, 0, 320, 213, {0x00FF0055});
    draw_texture(media1, 10, 10, 300, 193);

    // Text
    set_current_viewport(800 - 320, 800 - 213, 320, 213);
    p.n = 0xFF0000FF;

    // PRINT FPS
    uint32_t fps = FPS();
    texture_t FPS = create_text("FPS: " + STR(fps), p);
    draw_texture(FPS, 320 - (FPS.w + 5), 5);

    // Mouse
    mouse_t pos = mouse_position();
    texture_t mouse_pos_texture =
        create_text("X: " + STR(pos.x) + " Y: " + STR(pos.y), p);
    draw_texture(mouse_pos_texture, 2, 2);

    std::string left_button = "LEFT   old: " + pos_str(pos.left_old_state) +
                              " | new: " + pos_str(pos.left_new_state);

    std::string right_button = "RIGHT  old: " + pos_str(pos.right_old_state) +
                               " | new: " + pos_str(pos.right_new_state);

    texture_t mouse_left_button = create_text(left_button, p);
    draw_texture(mouse_left_button, 2, 30);
    texture_t mouse_right_button = create_text(right_button, p);
    draw_texture(mouse_right_button, 2, 60);
  }
};

int main()
{
  pixel p;

  if (p.run()) { return 0; }

  return 1;
}