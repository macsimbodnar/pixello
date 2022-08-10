#include "pixello.hpp"
#include <iostream>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_render.h"
#include "SDL_ttf.h"
#include "exceptions.hpp"


/*******************************************************************************
 * STRUCTS
 ******************************************************************************/

sdl_texture_wrapper_t::~sdl_texture_wrapper_t()
{
  if (ptr) { SDL_DestroyTexture(ptr); }
}


/*******************************************************************************
 * PIXELLO CLASS
 ******************************************************************************/

pixello::~pixello()
{
  if (_font) { TTF_CloseFont(_font); }
  if (_renderer) { SDL_DestroyRenderer(_renderer); }
  if (_window) { SDL_DestroyWindow(_window); }

  SDL_Quit();
}

void pixello::init()
{
  // INITIALIZATION
  int res;

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    throw init_exception("SDL could not initialize! SDL_Error: " +
                         std::string(SDL_GetError()));
  }

  // Set texture filtering to linear
  if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1") == false) {
    throw init_exception("Failed to setup the renderer: " +
                         std::string(SDL_GetError()));
  }

  // Init SDL IMG
  if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    throw init_exception("SDL_image could not initialize! SDL_image Error: " +
                         std::string(IMG_GetError()));
  }

  // Initialize SDL_ttf
  if (TTF_Init() == -1) {
    throw init_exception("SDL_ttf could not initialize! SDL_ttf Error: " +
                         std::string(TTF_GetError()));
  }

  // Create window
  _window = SDL_CreateWindow(_config.name.c_str(), SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, _config.window_w,
                             _config.window_h, SDL_WINDOW_SHOWN);

  if (_window == NULL) {
    throw init_exception("Window could not be created! SDL_Error: " +
                         std::string(SDL_GetError()));
  }

  // Get the window renderer
  _renderer = SDL_CreateRenderer(
      _window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (_renderer == NULL) {
    throw init_exception("Failed to create a window renderer! SDL_Error: " +
                         std::string(SDL_GetError()));
  }

  // Load the font id required
  if (_config.font_path.empty() == false) {
    _font = TTF_OpenFont(_config.font_path.c_str(), _config.font_size);

    if (_font == NULL) {
      throw init_exception("Failed to load the font! SDL_Error: " +
                           std::string(TTF_GetError()));
    }
  }

  // Initialize renderer color
  SDL_SetRenderDrawColor(_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

  // Enable Blend mode
  SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);

  // CALL THE USER INIT
  on_init();
}


bool pixello::run()
{
  try {
    init();

    SDL_Event event;
    bool running = true;
    uint32_t FPS_counter = 0;
    uint64_t FPS_last_check = SDL_GetPerformanceCounter();
    /*************************************************************
     *                          MAIN LOOP                        *
     *************************************************************/
    while (running) {
      const uint64_t start = SDL_GetPerformanceCounter();

      // POLL EVENTS
      while (SDL_PollEvent(&event)) {
        if ((SDL_QUIT == event.type) ||
            (SDL_KEYDOWN == event.type &&
             SDL_SCANCODE_ESCAPE == event.key.keysym.scancode)) {
          running = false;
          break;
        }
      }

      // Reset the viewport to entire window
      SDL_RenderSetViewport(_renderer, NULL);

      // USER UPDATE
      on_update();

      // DRAW
      SDL_RenderPresent(_renderer);

      // PERFORMANCE
      const uint64_t end = SDL_GetPerformanceCounter();
      const float freq = static_cast<float>(SDL_GetPerformanceFrequency());
      const float elapsed_s = (end - start) / freq;
      const float sleep_for_s = _config.target_s_per_frame - elapsed_s;
      const uint64_t sleep_for_ms =
          static_cast<uint64_t>(sleep_for_s * 1000.0f);

      // log("Sleep for: " + STR(sleep_for_ms) + "ms");

      if (sleep_for_s > 0) { SDL_Delay(sleep_for_ms); }
      //  else {
      //   log("Frame too slow");
      // }

      const float FPS_elapsed_s = (end - FPS_last_check) / freq;
      if (FPS_elapsed_s > 1.0f) {
        FPS_last_check = end;
        log("FPS: " + STR(FPS_counter));
        FPS_counter = 0;
      } else {
        ++FPS_counter;
      }
    }

  } catch (pixello_exception& e) {
    log("Exception: " + std::string(e.what()));
    return false;
  }

  return true;
}


void pixello::draw_pixel(int32_t x, int32_t y, pixel_t p)
{
  SDL_Rect rect = {x * _config.pixel_w, y * _config.pixel_h, _config.pixel_w,
                   _config.pixel_h};

  SDL_SetRenderDrawColor(_renderer, p.r, p.g, p.b, p.a);
  SDL_RenderFillRect(_renderer, &rect);
}


void pixello::clear_screen(pixel_t p)
{
  SDL_SetRenderDrawColor(_renderer, p.r, p.g, p.b, p.a);
  SDL_RenderClear(_renderer);
}


void pixello::draw_texture(const texture_t& t,
                           int32_t x,
                           int32_t y,
                           int32_t w,
                           int32_t h)
{
  SDL_Rect rect = {x, y, w, h};
  SDL_RenderCopy(_renderer, t.pointer(), NULL, &rect);
}


void pixello::draw_texture(const texture_t& t, int32_t x, int32_t y)
{
  draw_texture(t, x, y, t.w, t.h);
}


void pixello::set_current_viewport(int32_t x,
                                   int32_t y,
                                   int32_t w,
                                   int32_t h,
                                   pixel_t c)
{
  // Set viewport
  SDL_Rect rect = {x, y, w, h};
  SDL_RenderSetViewport(_renderer, &rect);

  // Set background color for view port
  SDL_Rect rect2 = {0, 0, w, h};
  SDL_SetRenderDrawColor(_renderer, c.r, c.g, c.b, c.a);
  SDL_RenderFillRect(_renderer, &rect2);
}


texture_t pixello::load_image(const std::string& img_path)
{
  texture_t t;

  SDL_Texture* tmp_ptr = IMG_LoadTexture(_renderer, img_path.c_str());

  if (tmp_ptr == NULL) {
    throw load_exceptions("Unable to load image to texture: " + img_path +
                          "! SDL Error: " + std::string(IMG_GetError()));
  }

  t._ptr = std::make_shared<sdl_texture_wrapper_t>(tmp_ptr);

  SDL_QueryTexture(t._ptr.get()->ptr, NULL, NULL, &t.w, &t.h);

  return t;
}


texture_t pixello::create_text(const std::string& text, pixel_t color)
{
  texture_t t;
  const SDL_Color c = {color.r, color.g, color.b, color.a};

  // Render text surface
  SDL_Surface* txt_surface = TTF_RenderText_Solid(_font, text.c_str(), c);

  if (txt_surface == NULL) {
    throw load_exceptions("Failed to generate the text surface: " + text +
                          " Error: " + std::string(TTF_GetError()));
  }

  // Create texture from surface pixels
  t._ptr = std::make_shared<sdl_texture_wrapper_t>(
      SDL_CreateTextureFromSurface(_renderer, txt_surface));

  if (!t._ptr) {
    // Free the surface in any case
    SDL_FreeSurface(txt_surface);

    throw load_exceptions("Failed to generate the text texture: " + text +
                          " Error: " + std::string(TTF_GetError()));
  }

  // Get image dimensions
  t.w = txt_surface->w;
  t.h = txt_surface->h;

  // Get rid of old surface
  SDL_FreeSurface(txt_surface);

  return t;
}