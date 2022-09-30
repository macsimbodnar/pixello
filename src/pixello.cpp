#include "pixello.hpp"
#include <SDL_mixer.h>
#include <iostream>
#include "SDL2_gfxPrimitives.h"
#include "SDL_image.h"
#include "SDL_render.h"
#include "SDL_ttf.h"


/*******************************************************************************
 * STRUCTS
 ******************************************************************************/

sdl_texture_wrapper_t::~sdl_texture_wrapper_t()
{
  if (ptr) {
    SDL_DestroyTexture(ptr);
    ptr = NULL;
  }
}

sdl_sound_wrapper_t::~sdl_sound_wrapper_t()
{
  if (music_ptr) {
    Mix_FreeMusic(music_ptr);
    music_ptr = NULL;
  }

  if (chunk_ptr) {
    Mix_FreeChunk(chunk_ptr);
    chunk_ptr = NULL;
  }
}


/*******************************************************************************
 * PIXELLO CLASS
 ******************************************************************************/

pixello::~pixello()
{
  if (_font) { TTF_CloseFont(_font); }
  if (_renderer) { SDL_DestroyRenderer(_renderer); }
  if (_window) { SDL_DestroyWindow(_window); }

  Mix_Quit();
  IMG_Quit();
  SDL_Quit();
}

void pixello::log(const std::string& msg)
{
  std::cout << msg << std::endl;
}

void pixello::init()
{
  // INITIALIZATION
  int res;

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
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

  // Init SDL Mixer
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    throw init_exception("SDL_mixer could not initialize! SDL_mixer Error: " +
                         std::string(Mix_GetError()));
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
  on_init(_external_data);
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

      // Reset did mouse moved flag
      _mouse_state.did_mouse_moved = false;

      // POLL EVENTS
      while (SDL_PollEvent(&event)) {
        switch (event.type) {
          case SDL_QUIT:
            running = false;
            break;

          case SDL_KEYDOWN:
            if (SDL_SCANCODE_ESCAPE == event.key.keysym.scancode) {
              running = false;
            }
            break;

          case SDL_MOUSEMOTION:
            SDL_GetMouseState(&_mouse_state.x, &_mouse_state.y);
            _mouse_state.did_mouse_moved = true;
            break;

          case SDL_MOUSEBUTTONDOWN:
            switch (event.button.button) {
              case SDL_BUTTON_LEFT:
                _mouse_state.left_button.state = key_t::DOWN;
                _mouse_state.left_button.click =
                    event.button.clicks == 1 ? true : false;
                _mouse_state.left_button.double_click =
                    event.button.clicks == 2 ? true : false;
                break;
              case SDL_BUTTON_RIGHT:
                _mouse_state.right_button.state = key_t::DOWN;
                _mouse_state.right_button.click =
                    event.button.clicks == 1 ? true : false;
                _mouse_state.right_button.double_click =
                    event.button.clicks == 2 ? true : false;
              case SDL_BUTTON_MIDDLE:
                _mouse_state.central_button.state = key_t::DOWN;
                _mouse_state.central_button.click =
                    event.button.clicks == 1 ? true : false;
                _mouse_state.central_button.double_click =
                    event.button.clicks == 2 ? true : false;
                break;
            }
            break;

          case SDL_MOUSEBUTTONUP:
            switch (event.button.button) {
              case SDL_BUTTON_LEFT:
                _mouse_state.left_button.state = key_t::UP;
                break;
              case SDL_BUTTON_RIGHT:
                _mouse_state.right_button.state = key_t::UP;
                break;
              case SDL_BUTTON_MIDDLE:
                _mouse_state.central_button.state = key_t::UP;
                break;
            }
            break;
        }
      }

      // Reset the viewport to entire window
      SDL_RenderSetViewport(_renderer, NULL);

      // USER UPDATE
      on_update(_external_data);

      // Reset mouse click state
      mouse_reset_clicks();

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
        // log("FPS: " + STR(FPS_counter));
        _FPS = FPS_counter;
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


void pixello::draw_pixel(const int32_t x,
                         const int32_t y,
                         const pixel_t& p) const
{
  SDL_Rect rect = {x * _config.pixel_size, y * _config.pixel_size,
                   _config.pixel_size, _config.pixel_size};

  SDL_SetRenderDrawColor(_renderer, p.r, p.g, p.b, p.a);
  SDL_RenderFillRect(_renderer, &rect);
}

void pixello::draw_rect(const rect_t& rect, const pixel_t& p) const
{
  SDL_SetRenderDrawColor(_renderer, p.r, p.g, p.b, p.a);
  SDL_RenderFillRect(_renderer, (SDL_Rect*)&rect);
}


void pixello::clear_screen(const pixel_t& p) const
{
  SDL_SetRenderDrawColor(_renderer, p.r, p.g, p.b, p.a);
  SDL_RenderClear(_renderer);
}


void pixello::draw_texture(const texture_t& t, const rect_t& rect) const
{
  SDL_RenderCopy(_renderer, t.pointer(), NULL, (SDL_Rect*)&rect);
}


void pixello::draw_texture(const texture_t& t,
                           const int32_t x,
                           const int32_t y) const
{
  const rect_t rect = {x, y, t.w, t.h};
  draw_texture(t, rect);
}


void pixello::music_do(const music_t::action_t action,
                       const music_t& music) const
{
  switch (action) {
    case music_t::PLAY:
      Mix_PlayMusic(music.pointer(), -1);
      break;

    case music_t::PAUSE:
      Mix_PausedMusic();
      break;

    case music_t::RESUME:
      Mix_ResumeMusic();
      break;

    case music_t::STOP:
      Mix_HaltMusic();
      break;
  }
}


void pixello::play_sound(const sound_t& sound) const
{
  Mix_PlayChannel(-1, sound.pointer(), 0);
}


void pixello::set_current_viewport(const rect_t& rect, const pixel_t& c)
{
  // Set viewport
  SDL_RenderSetViewport(_renderer, (SDL_Rect*)&rect);

  // Set background color for view port
  SDL_Rect rect2 = {0, 0, rect.w, rect.h};
  SDL_SetRenderDrawColor(_renderer, c.r, c.g, c.b, c.a);
  SDL_RenderFillRect(_renderer, &rect2);
}


texture_t pixello::load_image(const std::string& img_path) const
{
  SDL_Texture* tmp_ptr = IMG_LoadTexture(_renderer, img_path.c_str());

  if (!tmp_ptr) {
    throw load_exceptions("Unable to load image to texture: " + img_path +
                          "! SDL Error: " + std::string(IMG_GetError()));
  }

  texture_t t;
  t._ptr = std::make_shared<sdl_texture_wrapper_t>(tmp_ptr);

  SDL_QueryTexture(t._ptr.get()->ptr, NULL, NULL, &t.w, &t.h);

  return t;
}


texture_t pixello::create_text(const std::string& text,
                               const pixel_t& color) const
{
  const SDL_Color c = {color.r, color.g, color.b, color.a};

  // Render text surface
  SDL_Surface* txt_surface = TTF_RenderText_Solid(_font, text.c_str(), c);

  if (txt_surface == NULL) {
    throw load_exceptions("Failed to generate the text surface: " + text +
                          " Error: " + std::string(TTF_GetError()));
  }

  // Create texture from surface pixels
  auto tmp = SDL_CreateTextureFromSurface(_renderer, txt_surface);

  if (!tmp) {
    // Free the surface in any case
    SDL_FreeSurface(txt_surface);

    throw load_exceptions("Failed to generate the text texture: " + text +
                          " Error: " + std::string(TTF_GetError()));
  }

  texture_t t;
  t._ptr = std::make_shared<sdl_texture_wrapper_t>(tmp);

  // Get image dimensions
  t.w = txt_surface->w;
  t.h = txt_surface->h;

  // Get rid of old surface
  SDL_FreeSurface(txt_surface);

  return t;
}


sound_t pixello::load_sound(const std::string& sound_path) const
{
  auto tmp = Mix_LoadWAV(sound_path.c_str());

  if (!tmp) {
    throw load_exceptions("Failed to load sound! " + sound_path +
                          ". SDL_mixer Error: " + std::string(Mix_GetError()));
  }

  sound_t sound;
  sound._ptr = std::make_shared<sdl_sound_wrapper_t>(tmp);

  return sound;
}


music_t pixello::load_music(const std::string& music_path) const
{
  auto tmp = Mix_LoadMUS(music_path.c_str());

  if (!tmp) {
    throw load_exceptions("Failed to load music! " + music_path +
                          ". SDL_mixer Error: " + std::string(Mix_GetError()));
  }

  music_t music;
  music._ptr = std::make_shared<sdl_sound_wrapper_t>(tmp);

  return music;
}


bool pixello::is_mouse_in(const rect_t& rect) const
{
  return ((_mouse_state.x >= rect.x) && (_mouse_state.x < (rect.x + rect.w)) &&
          (_mouse_state.y >= rect.y) && (_mouse_state.y < (rect.y + rect.h)))
             ? true
             : false;
}


void pixello::draw_circle(const int32_t x,
                          const int32_t y,
                          const int32_t r,
                          const pixel_t& color) const
{
  filledCircleRGBA(_renderer, x, y, r, color.r, color.g, color.b, color.a);
}