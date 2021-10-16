#include "pixello.hpp"

#define CHECK(_RES_, _MSG_)                                                    \
  do {                                                                         \
    int _res = (_RES_);                                                        \
    if (_res) {                                                                \
      log(_MSG_);                                                              \
      return _res;                                                             \
    }                                                                          \
  } while (0)

pixello::pixello(config_t configuration) : config(std::move(configuration)) {}

bool pixello::run() {

  int res;

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    log("SDL could not initialize! SDL_Error: %s\n" +
        std::string(SDL_GetError()));
    return false;
  }

  // Create window
  window =
      SDL_CreateWindow(config.name.c_str(), config.window_x, config.window_y,
                       config.window_w, config.window_h, SDL_WINDOW_SHOWN);

  if (window == NULL) {
    log("Window could not be created! SDL_Error: %s\n" +
        std::string(SDL_GetError()));
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if (renderer == NULL) {
    log("Renderer could not be created! SDL_Error: %s\n" +
        std::string(SDL_GetError()));
    return false;
  }

  res = SDL_RenderSetLogicalSize(renderer, config.pixels_w, config.pixels_h);
  CHECK(res, "Failed to set renderer logical size");

  SDL_Event event;
  bool running = true;

  /*************************************************************
   *                          MAIN LOOP                        *
   *************************************************************/
  while (running) {
    const Uint64 start = SDL_GetPerformanceCounter();

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
    const Uint64 end = SDL_GetPerformanceCounter();
    const static Uint64 freq = SDL_GetPerformanceFrequency();
    const double seconds = (end - start) / static_cast<double>(freq);

    // log("Frame time: " + std::to_string(seconds * 1000.0) + "ms");

    // SDL_Delay(100);
  }

  return true;
}

pixello::~pixello() {
  // Destroy window
  if (window) {
    SDL_DestroyWindow(window);
  }

  // Quit SDL subsystems
  SDL_Quit();
}
