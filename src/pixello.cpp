#include "pixello.hpp"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_render.h"
#include <iostream>

// #define CHECK(_RES_, _MSG_)                                                    \
//   do {                                                                         \
//     int _res = (_RES_);                                                        \
//     if (_res) {                                                                \
//       log(_MSG_);                                                              \
//       return;                                                                  \
//     }                                                                          \
//   } while (0)

pixello::pixello(config_t configuration) : config(std::move(configuration)) {

  int res;

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    log("SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()));
    return;
  }

  // Create window
  window =
      SDL_CreateWindow(config.name.c_str(), config.window_x, config.window_y,
                       config.window_w, config.window_h, SDL_WINDOW_SHOWN);

  if (window == NULL) {
    log("Window could not be created! SDL_Error: " +
        std::string(SDL_GetError()));
    return;
  }

  // Get the window renderer
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    log("Failed to create a window renderer! SDL_Error: " +
        std::string(SDL_GetError()));
    return;
  }

  // Initialize renderer color
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
}

bool pixello::run() {
  const float target_s_per_frame = 1 / config.target_fps;

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

    // USER UPDATE
    on_update();

    // DRAW
    SDL_RenderPresent(renderer);

    // PERFORMANCE
    const uint64_t end = SDL_GetPerformanceCounter();
    const float freq = static_cast<float>(SDL_GetPerformanceFrequency());
    const float elapsed_s = (end - start) / freq;
    const float sleep_for_s = target_s_per_frame - elapsed_s;
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

pixello::texture_t pixello::load_texture(const std::string &path) {

  texture_t media;

  media.pointer = IMG_LoadTexture(renderer, path.c_str());

  if (media.pointer == NULL) {
    log("Unable to load image to texture: " + path +
        "! SDL Error: " + std::string(SDL_GetError()));
    // TODO(max): return with error or exception
  }

  return media;
}

void pixello::draw_texture(const pixello::texture_t &m) {
  SDL_RenderCopy(renderer, m.pointer, NULL, NULL);
}

void pixello::log(const std::string &msg) { std::cout << msg << std::endl; }