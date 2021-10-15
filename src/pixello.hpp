#pragma once

#include <inttypes.h>
#include <string>
#include <vector>

// Forward declaration to avoid include sdl in this header
struct SDL_Window;
struct SDL_Renderer;

class pixello {
private:
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;

  uint32_t window_x = 0;
  uint32_t window_y = 0;
  std::string name;

protected:
  uint32_t w = 640;
  uint32_t h = 480;

  virtual void log(const std::string &msg) = 0;
  virtual void on_update(std::vector<uint8_t> &pixels) = 0;

public:
  pixello(const std::string name, uint32_t width, uint32_t height, uint32_t x,
          uint32_t y);
  ~pixello();

  bool run();
};