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
struct rect_t
{
  int x, y;
  int w, h;
};

struct pixel_t
{
  union
  {
    uint32_t n;  // 0xRRGGGBBBAAA
    struct
    {
      uint8_t a;
      uint8_t b;
      uint8_t g;
      uint8_t r;
    };
  };

  pixel_t() : n(0x000000FF) {}
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
  int32_t pixel_size;

  uint32_t window_w;
  uint32_t window_h;

  const std::string name;

  float target_fps;
  float target_s_per_frame;

  int32_t width_in_pixels;
  int32_t height_in_pixels;

  std::string font_path;
  int32_t font_size;

  config_t(uint32_t ps,
           uint32_t ww,
           uint32_t wh,
           std ::string wname,
           float Hz,
           std::string font,
           int32_t font_s)
      : pixel_size(ps),
        window_w(ww),
        window_h(wh),
        name(std::move(wname)),
        target_fps(Hz),
        target_s_per_frame(1 / target_fps),
        width_in_pixels(window_w / pixel_size),
        height_in_pixels(window_h / pixel_size),
        font_path(std::move(font)),
        font_size(font_s)
  {
    if (pixel_size < 1) { pixel_size = 1; }
  }
};

struct button_t
{
  enum state_t
  {
    DOWN,
    UP
  };

  state_t state = UP;
  bool click = false;
  bool double_click = false;
};

struct mouse_t
{
  int32_t x = 0;
  int32_t y = 0;
  int32_t x_delta = 0;
  int32_t y_delta = 0;

  button_t left_button;
  button_t central_button;
  button_t right_button;
};

/*******************************************************************************
 * PIXELLO CLASS
 ******************************************************************************/
class pixello
{
private:
  uint32_t _FPS = 0;
  mouse_t _old_mouse_state = {0};
  mouse_t _mouse_state = {0};

  SDL_Window* _window = NULL;
  SDL_Renderer* _renderer = NULL;
  _TTF_Font* _font = NULL;

  config_t _config;

  void* _external_data = nullptr;

  void init();

protected:
  // Have to Override this
  virtual void on_update(void* external_data) = 0;
  virtual void on_init(void* external_data) = 0;

  // You can override this if you want
  virtual void log(const std::string& msg);

public:
  pixello(uint32_t ww,
          uint32_t wh,
          std ::string wname,
          float Hz,
          std::string font,
          int32_t font_s,
          void* external_data = nullptr)
      : _config({1, ww, wh, wname, Hz, font, font_s}),
        _external_data(external_data)
  {}

  pixello(uint32_t pixel_size,
          uint32_t ww,
          uint32_t wh,
          std ::string wname,
          float Hz,
          std::string font,
          int32_t font_s,
          void* external_data = nullptr)
      : _config({pixel_size, ww, wh, wname, Hz, font, font_s}),
        _external_data(external_data)
  {}

  ~pixello();

  bool run();

  // Routines
  void draw_pixel(int32_t x, int32_t y, const pixel_t& p);
  void draw_rect(const rect_t& rect, const pixel_t& p);
  void clear_screen(const pixel_t& p);

  void draw_texture(const texture_t& t, int32_t x, int32_t y);
  void draw_texture(const texture_t& t, const rect_t& rect);

  texture_t load_image(const std::string& img_path);
  texture_t create_text(const std::string& text, const pixel_t& color);

  inline int32_t width_in_pixels() const { return _config.width_in_pixels; }
  inline int32_t height_in_pixels() const { return _config.height_in_pixels; }
  inline int32_t width() const { return _config.window_w; }
  inline int32_t height() const { return _config.window_h; }
  inline mouse_t mouse_state() const { return _mouse_state; }
  inline void mouse_reset_clicks()
  {
    _mouse_state.left_button.click = false;
    _mouse_state.left_button.double_click = false;
    _mouse_state.right_button.click = false;
    _mouse_state.right_button.double_click = false;
    _mouse_state.central_button.click = false;
    _mouse_state.central_button.double_click = false;
  }

  inline uint32_t FPS() const { return _FPS; }

  void set_current_viewport(const rect_t& rect,
                            const pixel_t& color = {0x555555FF});

  bool is_mouse_in(const rect_t& rect) const;
};
