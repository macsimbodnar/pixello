#include <iostream>
#include <vector>
#include "pixello.hpp"

texture_t media1;
texture_t media2;
texture_t sprites;

int32_t media2_x;
int32_t media2_y;

sound_t sound;
music_t music;

font_t font;
font_t font_2;

int32_t x_clip_pos = 800 / 2;

bool holding_icon;
int32_t holding_offset_x;
int32_t holding_offset_y;

uint32_t click_counter = 0;
uint32_t double_click_counter = 0;

bool hide_mouse = false;

float sound_volume = 1.0f;
float music_volume = 1.0f;
float global_volume = 1.0f;
bool music_state = false;
simple_timer timer;

std::string pos_str(button_key_t::state_t s)
{
  std::string res = "";
  switch (s) {
    case button_key_t::DOWN:
      res = "down";
      break;
    case button_key_t::UP:
      res = "up";
      break;
  }

  return res;
}

class pixel : public pixello
{
public:
  pixel() : pixello(20, 800, 800, "Test pixello", 60) {}

private:
  void log(const std::string& msg) override { std::cout << msg << std::endl; }

  void on_click()
  {
    mouse_set_FPS_mode(hide_mouse);
    hide_mouse = !hide_mouse;
    log("Click! " + std::to_string(hide_mouse));
  }

  void on_init(void*) override
  {
    font = load_font("assets/font/PressStart2P.ttf", 10);
    font_2 = load_font("assets/font/PressStart2P.ttf", 20);
    media1 = load_image("assets/sample_640x426.bmp");
    media2 = load_image("assets/Chess_klt60.png");
    sprites = load_image("assets/sprites.png");

    media2_x = (800 / 2) - (media2.w / 2);
    media2_y = (800 / 2) - (media2.h / 2);

    holding_icon = false;
    holding_offset_x = 0;
    holding_offset_y = 0;

    music = load_music("assets/sound/doom.wav");
    sound = load_sound("assets/sound/dspunch.wav");
  }

  void on_update(void*) override
  {
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

    // Draw rect outline
    center = {width() / 2 - 90, height() / 2 - 90, 180, 180};
    draw_rect_outline(center, 0x000000FF);

    // Draw H line
    point_t a = {0, height() / 2};
    point_t b = {width(), height() / 2};
    draw_line(a, b, 0xFFFFFFFF);

    // Draw dots
    int num_of_dots = 10;
    for (int i = 0; i < num_of_dots; ++i) {
      point_t p = {width() / 2 - 80 + (i * 10), height() / 2 - 80};
      draw_dot(p, 0xFFFFFFFF);
    }

    // Draw a clip based on the mouse relative pos
    x_clip_pos += mouse_state().relative_x * 2;
    draw_texture(sprites, {x_clip_pos, height() / 2, 50, 50}, {0, 0, 100, 100});

    // Update media2 position if mouse current state button down
    const rect_t r = {media2_x, media2_y, media2.w, media2.h};
    if (is_mouse_in(r) &&
        (mouse_state().left_button.state == button_key_t::DOWN)) {
      if (!holding_icon) {
        holding_offset_x = mouse_state().x - media2_x;
        holding_offset_y = mouse_state().y - media2_y;
      }
      holding_icon = true;
    }

    if (mouse_state().left_button.state == button_key_t::UP) {
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

    // Draw circle
    draw_circle(800 - 50, 50, 50, 0xFF000055);

    // View port, Images
    // set_current_viewport({0, 0, 320, 213}, 0x00FF0055);
    draw_texture(media1, {10, 10, 300, 193});

    // Text
    rect_t gray_viewport = {800 - 320, 800 - 213, 320, 213};
    // set_current_viewport(gray_viewport);
    draw_rect(gray_viewport, 0x555555FF);
    p.n = 0xFF0000FF;

    // PRINT FPS
    uint32_t fps = FPS();
    texture_t FPS = create_text("FPS: " + STR(fps), p, font);
    draw_texture(FPS, gray_viewport.x + 320 - FPS.w, gray_viewport.y + 0);

    // Print Delta T
    uint64_t dt = delta_time();
    texture_t DT = create_text("DT: " + STR(dt), p, font);
    draw_texture(DT, gray_viewport.x + 320 - DT.w, gray_viewport.y + FPS.h + 2);

    // Print FOnt 2
    texture_t f2 = create_text("Font 2", p, font_2);
    draw_texture(f2, gray_viewport.x + 320 - f2.w, gray_viewport.y + f2.h + 6);

    // Print WASD KEYS
    if (is_key_pressed(keycap_t::W)) {
      texture_t T = create_text("W", p, font);
      draw_texture(T, gray_viewport.x + 0, gray_viewport.y + 213 - T.h);
      music_volume += 0.05;
      set_music_volume(music_volume);
    }
    if (is_key_pressed(keycap_t::S)) {
      texture_t T = create_text("S", p, font);
      draw_texture(T, gray_viewport.x + 20, gray_viewport.y + 213 - T.h);
      music_volume -= 0.05;
      set_music_volume(music_volume);
    }

    if (is_key_pressed(keycap_t::A)) {
      texture_t T = create_text("A", p, font);
      draw_texture(T, gray_viewport.x + 40, gray_viewport.y + 213 - T.h);
      sound_volume -= 0.05f;
      set_sound_volume(sound_volume);
    }

    if (is_key_pressed(keycap_t::D)) {
      texture_t T = create_text("D", p, font);
      draw_texture(T, gray_viewport.x + 60, gray_viewport.y + 213 - T.h);
      sound_volume += 0.05f;
      set_sound_volume(sound_volume);
    }

    if (is_key_pressed(keycap_t::L)) {
      global_volume += 0.05f;
      set_master_volume(global_volume);
    }

    if (is_key_pressed(keycap_t::K)) {
      global_volume -= 0.05f;
      set_master_volume(global_volume);
    }

    int32_t y_draw_offset = 0;
    // Mouse
    mouse_t state = mouse_state();
    texture_t mouse_pos_texture =
        create_text("X: " + STR(state.x) + " Y: " + STR(state.y), p, font);
    draw_texture(mouse_pos_texture, gray_viewport.x + 0,
                 gray_viewport.y + y_draw_offset);

    y_draw_offset += mouse_pos_texture.h;

    texture_t left_button_key_texture =
        create_text("MLB: " + pos_str(state.left_button.state), p, font);
    draw_texture(left_button_key_texture, gray_viewport.x + 0,
                 gray_viewport.y + y_draw_offset);

    y_draw_offset += left_button_key_texture.h;

    if (mouse_state().left_button.click) { ++click_counter; }

    if (mouse_state().left_button.double_click) { ++double_click_counter; }

    texture_t click_counter_texture =
        create_text("Clicks: " + STR(click_counter), p, font);
    draw_texture(click_counter_texture, gray_viewport.x + 0,
                 gray_viewport.y + y_draw_offset);

    y_draw_offset += click_counter_texture.h;

    texture_t double_click_counter_texture =
        create_text("Double clicks: " + STR(double_click_counter), p, font);
    draw_texture(double_click_counter_texture, gray_viewport.x + 0,
                 gray_viewport.y + y_draw_offset);

    y_draw_offset += double_click_counter_texture.h;

    texture_t did_mouse_moved_texture =
        create_text("Moved: " + STR(mouse_state().did_mouse_moved), p, font);
    draw_texture(did_mouse_moved_texture, gray_viewport.x + 0,
                 gray_viewport.y + y_draw_offset);

    y_draw_offset += did_mouse_moved_texture.h;

    set_sound_volume(sound, 0.5f);

    // Button
    rect_t button_rect = {gray_viewport.x + 0, gray_viewport.y + 100, 100, 30};
    texture_t button_text = create_text("Button", 0x000000FF, font);
    button_t button =
        create_button(button_rect, 0xFFFFFFFF, button_text, 0x111111FF);

    if (is_mouse_in(button_rect) && mouse_state().left_button.click) {
      on_click();

      if (music_state) {
        pause_music();
        music_state = !music_state;
      } else {
        resume_music();
        music_state = !music_state;
      }
    }

    draw_button(button);

    // Sound
    if (is_mouse_in(gray_viewport)) {
      if (mouse_state().left_button.click) { play_sound(sound); }
      if (mouse_state().right_button.click) {
        play_music(music);
        music_state = true;
      }
    }

    if (timer.get_ticks() >= 5 * 1000) {
      log("5s elapsed");
      timer.restart();
    }

    if (is_key_pressed(keycap_t::T)) {
      if (!timer.is_started()) { timer.start(); }
    }

    if (is_key_pressed(keycap_t::S)) {
      if (timer.is_started()) { timer.stop(); }
    }

    if (is_key_pressed(keycap_t::P)) {
      if (!timer.is_paused()) { timer.pause(); }
    }


    if (is_key_pressed(keycap_t::U)) {
      if (timer.is_paused()) { timer.unpause(); }
    }

    // Check if we have to quit the game
    if (is_key_pressed(keycap_t::ESC)) { stop(); }
  }
};

int main()
{
  pixel p;

  if (p.run()) { return 0; }

  return 1;
}