#pragma once

#include <inttypes.h>
#include <memory>
#include <string>

// Forward declaration to avoid include sdl in this header
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
struct _TTF_Font;

/*******************************************************************************
 * MACRO
 ******************************************************************************/
#define STR(_N_) std::to_string(_N_)

/*******************************************************************************
 * STRUCTS
 ******************************************************************************/
struct pixel_t
{
  union
  {
    uint32_t n;  // Little endian ABGR
    struct
    {
      uint8_t r;
      uint8_t g;
      uint8_t b;
      uint8_t a;
    };
  };

  pixel_t() : n(0xFF000000) {}
  pixel_t(uint32_t color) : n(color) {}
  pixel_t(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
      : r(red), g(green), b(blue), a(alpha)
  {}
};

struct sdl_texture_wrapper_t
{
  SDL_Texture* ptr = NULL;

  sdl_texture_wrapper_t(SDL_Texture* p) : ptr(p) {}
  ~sdl_texture_wrapper_t();
};

struct texture_t
{
  int32_t w = 0;
  int32_t h = 0;

  std::shared_ptr<sdl_texture_wrapper_t> _ptr;

  texture_t() {}
  inline SDL_Texture* pointer() const { return _ptr.get()->ptr; }
};

struct config_t
{
  int32_t pixel_w;
  int32_t pixel_h;

  uint32_t window_w;
  uint32_t window_h;

  const std::string name;

  float target_fps;
  float target_s_per_frame;

  int32_t width_in_pixels;
  int32_t height_in_pixels;

  std::string font_path;
  int32_t font_size;

  config_t(uint32_t pw,
           uint32_t ph,
           uint32_t ww,
           uint32_t wh,
           std ::string wname,
           float Hz,
           std::string font,
           int32_t font_s)
      : pixel_w(pw),
        pixel_h(ph),
        window_w(ww),
        window_h(wh),
        name(std::move(wname)),
        target_fps(Hz),
        target_s_per_frame(1 / target_fps),
        width_in_pixels(window_w / pixel_w),
        height_in_pixels(window_h / pixel_h),
        font_path(std::move(font)),
        font_size(font_s)
  {}
};

struct mouse_t
{
  enum state_t
  {
    REST,
    DOWN,
    UP
  };

  int32_t x = 0;
  int32_t y = 0;

  state_t left_old_state = REST;
  state_t left_new_state = REST;
  state_t right_old_state = REST;
  state_t right_new_state = REST;
};

/*******************************************************************************
 * PIXELLO CLASS
 ******************************************************************************/
class pixello
{
private:
  uint32_t _FPS = 0;
  mouse_t _mouse_position = {0};

  SDL_Window* _window = NULL;
  SDL_Renderer* _renderer = NULL;
  _TTF_Font* _font = NULL;

  config_t _config;

protected:
  // Override this
  virtual void log(const std::string& msg) = 0;
  virtual void on_update() = 0;
  virtual void on_init() = 0;

private:
  void init();

public:
  pixello(config_t configuration) : _config(std::move(configuration)) {}
  ~pixello();

  bool run();

  // Routines
  void draw_pixel(int32_t x, int32_t y, pixel_t p);
  void clear_screen(pixel_t p);

  void draw_texture(const texture_t& t, int32_t x, int32_t y);
  void draw_texture(const texture_t& t,
                    int32_t x,
                    int32_t y,
                    int32_t w,
                    int32_t h);

  texture_t load_image(const std::string& img_path);
  texture_t create_text(const std::string& text, pixel_t color);

  inline int32_t width_in_pixels() const { return _config.width_in_pixels; }
  inline int32_t height_in_pixels() const { return _config.height_in_pixels; }
  inline mouse_t mouse_position() const { return _mouse_position; }

  inline uint32_t FPS() const { return _FPS; }

  void set_current_viewport(int32_t x,
                            int32_t y,
                            int32_t w,
                            int32_t h,
                            pixel_t color = {0xFF555555});
};
