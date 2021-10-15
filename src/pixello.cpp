#include "pixello.hpp"
#include "SDL.h"
#include "SDL_render.h"

pixello::pixello(const std::string w_name, uint32_t width, uint32_t height,
                 uint32_t x, uint32_t y)
    : w(width), h(height), window_x(x), window_y(y), name(std::move(w_name)) {}

bool pixello::run() {

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    log("SDL could not initialize! SDL_Error: %s\n" +
        std::string(SDL_GetError()));
    return false;
  }

  // Create window
  window = SDL_CreateWindow(name.c_str(), window_x, window_y, w, h,
                            SDL_WINDOW_SHOWN);

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

  SDL_RendererInfo info;
  SDL_GetRendererInfo(renderer, &info);
  log("Renderer name: " + std::string(info.name));
  log("Texture formats: ");

  for (Uint32 i = 0; i < info.num_texture_formats; i++) {
    log(std::string(SDL_GetPixelFormatName(info.texture_formats[i])));
  }

  SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STREAMING, w, h);

  std::vector<uint8_t> pixels(w * h * 4, 0);

  SDL_Event event;
  bool running = true;

  while (running) {
    const Uint64 start = SDL_GetPerformanceCounter();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    while (SDL_PollEvent(&event)) {
      if ((SDL_QUIT == event.type) ||
          (SDL_KEYDOWN == event.type &&
           SDL_SCANCODE_ESCAPE == event.key.keysym.scancode)) {
        running = false;
        break;
      }
    }

    on_update(pixels);

    // uint8_t* lockedPixels;
    // int32_t pitch;
    // SDL_LockTexture
    //     (
    //     texture,
    //     NULL,
    //     reinterpret_cast< void** >( &lockedPixels ),
    //     &pitch
    //     );
    // std::copy( pixels.begin(), pixels.end(), lockedPixels );
    // SDL_UnlockTexture( texture );

    SDL_UpdateTexture(texture, NULL, pixels.data(), w * 4);

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    const Uint64 end = SDL_GetPerformanceCounter();
    const static Uint64 freq = SDL_GetPerformanceFrequency();
    const double seconds = (end - start) / static_cast<double>(freq);

    // log("Frame time: " + std::to_string(seconds * 1000.0) + "ms");
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