#include "pixello.hpp"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_render.h"
#include "SDL_ttf.h"
#include <iostream>

// #define CHECK(_RES_, _MSG_)                                                    \
//   do {                                                                         \
//     int _res = (_RES_);                                                        \
//     if (_res) {                                                                \
//       log(_MSG_);                                                              \
//       return;                                                                  \
//     }                                                                          \
//   } while (0)

pixello::pixello(config_t configuration) : config(std::move(configuration)) {}

bool pixello::run() {

  /*************************************************************
   *                          INITIALIZATION                   *
   *************************************************************/
  int res;

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    log("SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()));
    return false;
  }

  // Set texture filtering to linear
  if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1") == false) {
    log("Failed to setup the renderer: " + std::string(SDL_GetError()));
    return false;
  }

  // Init SDL IMG
  if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    log("SDL_image could not initialize! SDL_image Error: " +
        std::string(IMG_GetError()));
    return false;
  }

  // Initialize SDL_ttf
  if (TTF_Init() == -1) {
    log("SDL_ttf could not initialize! SDL_ttf Error: " +
        std::string(TTF_GetError()));
    return false;
  }

  // Create window
  window = SDL_CreateWindow(config.name.c_str(), SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, config.window_w,
                            config.window_h, SDL_WINDOW_SHOWN);

  if (window == NULL) {
    log("Window could not be created! SDL_Error: " +
        std::string(SDL_GetError()));
    return false;
  }

  // Get the window renderer
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (renderer == NULL) {
    log("Failed to create a window renderer! SDL_Error: " +
        std::string(SDL_GetError()));
    return false;
  }

  // Load the font id required
  if (config.font_path.empty() == false) {
    font = TTF_OpenFont(config.font_path.c_str(), config.font_size);

    if (font == NULL) {
      log("Failed to load the font! SDL_Error: " + std::string(TTF_GetError()));
      return false;
    }
  }

  // Initialize renderer color
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

  // CALL THE USER INIT
  on_init();

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
    SDL_RenderSetViewport(renderer, NULL);

    // USER UPDATE
    on_update();

    // DRAW
    SDL_RenderPresent(renderer);

    // PERFORMANCE
    const uint64_t end = SDL_GetPerformanceCounter();
    const float freq = static_cast<float>(SDL_GetPerformanceFrequency());
    const float elapsed_s = (end - start) / freq;
    const float sleep_for_s = config.target_s_per_frame - elapsed_s;
    const uint64_t sleep_for_ms = static_cast<uint64_t>(sleep_for_s * 1000.0f);

    // log("Sleep for: " + STR(sleep_for_ms) + "ms");

    if (sleep_for_s > 0) {
      SDL_Delay(sleep_for_ms);
    }
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

  return true;
}

pixello::~pixello() {

  if (font) {
    TTF_CloseFont(font);
  }

  if (renderer) {
    SDL_DestroyRenderer(renderer);
  }

  if (window) {
    SDL_DestroyWindow(window);
  }

  SDL_Quit();
}

// Routines
void pixello::draw(int32_t x, int32_t y, pixel_t p) {

  SDL_Rect rect = {x * config.pixel_w, y * config.pixel_h, config.pixel_w,
                   config.pixel_h};

  SDL_SetRenderDrawColor(renderer, p.r, p.g, p.b, p.a);
  SDL_RenderFillRect(renderer, &rect);
}

void pixello::clear(pixel_t p) {
  SDL_SetRenderDrawColor(renderer, p.r, p.g, p.b, p.a);
  SDL_RenderClear(renderer);
}

void pixello::draw_texture(const pixello::texture &t, int32_t x, int32_t y,
                           int32_t w, int32_t h) {

  SDL_Rect rect = {x, y, w, h};
  SDL_RenderCopy(renderer, t.pointer(), NULL, &rect);
}

void pixello::draw_texture(const texture &t, int32_t x, int32_t y) {
  draw_texture(t, x, y, t.width(), t.height());
}

void pixello::draw_text(const text &t, int32_t x, int32_t y, int32_t w,
                        int32_t h) {
  SDL_Rect rect = {x, y, w, h};
  SDL_RenderCopy(renderer, t.pointer(), NULL, &rect);
}

void pixello::draw_text(const text &t, int32_t x, int32_t y) {
  draw_text(t, x, y, t.width(), t.height());
}

void pixello::set_current_viewport(int32_t x, int32_t y, int32_t w, int32_t h) {

  // Set viewport
  SDL_Rect rect = {x, y, w, h};
  SDL_RenderSetViewport(renderer, &rect);

  // Set background color for view port
  SDL_Rect rect2 = {0, 0, w, h};
  SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0x55, 0xFF);
  SDL_RenderFillRect(renderer, &rect2);
}

pixello::texture::texture(pixello *p, const std::string &path) {

  ptr = std::make_shared<sdl_texture_wrapper>(
      IMG_LoadTexture(p->renderer, path.c_str()));

  if (ptr == NULL) {
    p->log("Unable to load image to texture: " + path +
           "! SDL Error: " + std::string(IMG_GetError()));
    // TODO(max): return with error or exception
  }

  SDL_QueryTexture(ptr.get()->ptr, NULL, NULL, &w, &h);
}

pixello::sdl_texture_wrapper::~sdl_texture_wrapper() {
  if (ptr) {
    SDL_DestroyTexture(ptr);
  }
}

pixello::texture pixello::load_texture(const std::string &path) {
  return texture(this, path);
}

pixello::text pixello::create_text(const std::string &t, pixel_t color) {
  return text(this, t, color);
}

pixello::text::text(pixello *p, const std::string &text, pixel_t color) {
  SDL_Color c = {color.r, color.g, color.b, color.a};

  // Render text surface
  SDL_Surface *txt_surface = TTF_RenderText_Solid(p->font, text.c_str(), c);

  if (txt_surface == NULL) {
    p->log("Failed to generate the text surface: " + text +
           " Error: " + std::string(TTF_GetError()));
    return;
  }

  // Create texture from surface pixels
  ptr = std::make_shared<sdl_texture_wrapper>(
      SDL_CreateTextureFromSurface(p->renderer, txt_surface));

  if (ptr == NULL) {
    p->log("Failed to generate the text texture: " + text +
           " Error: " + std::string(TTF_GetError()));

    // Free the surface in any case
    SDL_FreeSurface(txt_surface);
  }
  // Get image dimensions
  w = txt_surface->w;
  h = txt_surface->h;

  // Get rid of old surface
  SDL_FreeSurface(txt_surface);
}
