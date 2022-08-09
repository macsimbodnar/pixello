#pragma once

#include <inttypes.h>
#include <string>

// Forward declaration to avoid include sdl in this header
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

#define STR(_N_) std::to_string(_N_)

class pixello {
public:
  struct pixel_t {
    union {
      uint32_t n = 0xFF000000;
      struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
      };
    };
  };

  struct texture_t {
    SDL_Texture *pointer;
  };

private:
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;

  struct config_t {
    int32_t pixel_w;
    int32_t pixel_h;

    uint32_t window_w;
    uint32_t window_h;

    const std::string name;

    float target_fps;
    float target_s_per_frame;

    int32_t width_in_pixels;
    int32_t height_in_pixels;

    config_t(uint32_t pw, uint32_t ph, uint32_t ww, uint32_t wh,
             std ::string wname, float Hz)
        : pixel_w(pw), pixel_h(ph), window_w(ww), window_h(wh),
          name(std::move(wname)), target_fps(Hz),
          target_s_per_frame(1 / target_fps),
          width_in_pixels(window_w / pixel_w),
          height_in_pixels(window_h / pixel_h) {}
  };

  config_t config;

protected:
  virtual void log(const std::string &msg) = 0;
  virtual void on_update() = 0;
  virtual void on_init() = 0;

public:
  pixello(config_t configuration);
  ~pixello();

  bool run();

  // Routines
  void draw(int32_t x, int32_t y, pixel_t p);
  void clear(pixel_t p);

  void draw_texture(const texture_t &m);
  texture_t load_texture(const std::string &path);

  inline int32_t width_in_pixels() const { return config.width_in_pixels; }
  inline int32_t height_in_pixels() const { return config.height_in_pixels; }

  void set_current_viewport(int32_t x, int32_t y, int32_t w, int32_t h);
};
