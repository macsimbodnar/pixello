#include <iostream>
#include <vector>
#include "pixello.hpp"

texture_t media1;
texture_t media2;

int32_t media2_x;
int32_t media2_y;

bool holding_icon;
int32_t holding_offset_x;
int32_t holding_offset_y;

std::string pos_str(button_t::state_t s)
{
  std::string res = "";
  switch (s) {
    case button_t::DOWN:
      res = "down";
      break;
    case button_t::UP:
      res = "up";
      break;
  }

  return res;
}

class pixel : public pixello
{
public:
  pixel()
      : pixello(20,
                800,
                800,
                "Test pixello",
                60,
                "assets/font/PressStart2P.ttf",
                10)
  {}

private:
  void log(const std::string& msg) override { std::cout << msg << std::endl; }

  void on_init(void* data) override
  {
    media1 = load_image("assets/sample_640x426.bmp");
    media2 = load_image("assets/Chess_klt60.png");

    media2_x = (800 / 2) - (media2.w / 2);
    media2_y = (800 / 2) - (media2.h / 2);

    holding_icon = false;
    holding_offset_x = 0;
    holding_offset_y = 0;
  }

  void on_update(void* data) override
  {
    clear_screen(0x000000FF);

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

    // Draw a rect
    rect_t center = {width() / 2 - 100, height() / 2 - 100, 200, 200};
    draw_rect(center, 0xFF00FFFF);


    // Update media2 position if mouse current state button down
    const rect_t r = {media2_x, media2_y, media2.w, media2.h};
    if (is_mouse_in(r) &&
        (mouse_state().left_button.state == button_t::DOWN)) {
      if (!holding_icon) {
        holding_offset_x = mouse_state().x - media2_x;
        holding_offset_y = mouse_state().y - media2_y;
      }
      holding_icon = true;
    }

    if (mouse_state().left_button.state == button_t::UP) {
      holding_icon = false;
      holding_offset_x = 0;
      holding_offset_y = 0;
    }

    if (holding_icon) {
      media2_x = mouse_state().x - holding_offset_x;
      media2_y = mouse_state().y - holding_offset_y;
    }

    // Draw texture with transparency
    draw_texture(media2, media2_x, media2_y);

    // View port, Images
    set_current_viewport({0, 0, 320, 213}, 0x00FF0055);
    draw_texture(media1, {10, 10, 300, 193});

    // Text
    set_current_viewport({800 - 320, 800 - 213, 320, 213});
    p.n = 0xFF0000FF;

    // PRINT FPS
    uint32_t fps = FPS();
    texture_t FPS = create_text("FPS: " + STR(fps), p);
    draw_texture(FPS, 320 - (FPS.w + 5), 5);

    // Mouse
    mouse_t state = mouse_state();
    texture_t mouse_pos_texture =
        create_text("X: " + STR(state.x) + " Y: " + STR(state.y), p);
    draw_texture(mouse_pos_texture, 0, 0);

    texture_t left_button_texture =
        create_text("MLB: " + pos_str(state.left_button.state), p);
    draw_texture(left_button_texture, 0, 20);
  }
};

int main()
{
  pixel p;

  if (p.run()) { return 0; }

  return 1;
}