#pragma once

#include <inttypes.h>
#include <string>

// Forward declaration to avoid include sdl in this header
struct SDL_Window;
struct SDL_Renderer;

// Log function prototype
typedef void (*log_func)(const std::string &log);

class pixello {
private:
  log_func log_callback = nullptr;

  int32_t w = 640;
  int32_t h = 480;
  uint32_t window_x = 0;
  uint32_t window_y = 0;
  std::string name;

  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;

  void log(const std::string &msg);

public:
  pixello(const std::string name, int32_t width, int32_t height, uint32_t x,
          uint32_t y);

  void set_logger(log_func log_callback);
  bool run();
  bool close();
};