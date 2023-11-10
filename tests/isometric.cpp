#include <math.h>
#include "pixello.hpp"

constexpr int screen_w = 800;
constexpr int screen_h = 400;

// Number of tiles in world
constexpr int world_w = 20;
constexpr int world_h = 20;

// Size of single tile graphic
constexpr int tile_w = 40;
constexpr int tile_h = 20;

// Where to place tile (0,0) on screen (in tile size steps)
point_t origin = {5, 1};

// Sprite that holds all imagery
constexpr int sprite_w = 40;
constexpr int sprite_h = 20;
texture_t sprites;
font_t font;

// Pointer to create 2D world array
int world[world_h][world_w];


point_t screen_to_isometric(const int x, const int y)
{
  point_t result;
  // result.x = (y / tile_h) + (x / tile_w);
  // result.y = (y / tile_h) - (x / tile_w);

  // const float x = _x;
  // const float y = _y;
  const int tile_w_half = tile_w / 2;
  const int tile_h_half = tile_h / 2;

  // result.x = floorf(0.5 * (x / tile_w_half + y / tile_h_half));
  // result.y = floorf(0.5 * (-x / tile_w_half + y / tile_h_half));
  int tile_x = x - 2 * y + tile_w_half;
  int tile_y = x + 2 * y - tile_h_half;

  tile_x /= tile_w;
  tile_y /= tile_h;

  result.x = tile_x;
  result.y = tile_y;

  return result;
}


class isometric : public pixello
{
public:
  isometric() : pixello(20, screen_w, screen_h, "Test pixello", 60) {}

private:
  void on_init(void*) override
  {
    log("on init");

    // Load sprites used in demonstration
    sprites = load_image("assets/isometric_demo.png");

    // Load font
    font = load_font("assets/font/PressStart2P.ttf", 10);

    // Create empty world
    int i = 0;
    for (auto& row : world) {
      for (auto& element : row) {
        element = i % 6;
      }
      ++i;
    }
  }

  inline static point_t to_screen_pos(const point_t& p)
  {
    point_t res;
    res.x = (origin.x * tile_w) + (p.x - p.y) * (tile_w / 2);
    res.y = (origin.y * tile_h) + (p.x + p.y) * (tile_h / 2);
    return res;
  }

  void on_update(void*) override
  {
    {  // Useless
      if (mouse_state().left_button.click) { log("Click!"); }

      // Check if we have to quit the game
      if (is_key_pressed(keycap_t::ESC)) { stop(); }
    }

    // Clear screen
    draw_rect({0, 0, 800, 800}, 0xFFFFFFFF);

    // Get Mouse in world
    const int mouse_x = mouse_state().x;
    const int mouse_y = mouse_state().y;

    // Work out active cell
    const int active_cell_x = mouse_x / tile_w;
    const int active_cell_y = mouse_y / tile_h;

    const point_t selected_tile = screen_to_isometric(mouse_x, mouse_y);

    // Work out mouse offset into cell
    // const int offset_x = mouse_x % tile_w;
    // const int offset_y = mouse_y % tile_h;

    for (int y = 0; y < world_h; ++y) {
      for (int x = 0; x < world_h; ++x) {
        const point_t screen_pos = to_screen_pos({x, y});

        const int cell_elem = world[y][x];
        rect_t position;
        rect_t sprite_crop;
        switch (cell_elem) {
          default:
          case 0:
            // Invisible Tile
            position.x = screen_pos.x;
            position.y = screen_pos.y;
            position.w = tile_w;
            position.h = tile_h;

            sprite_crop.x = 1 * sprite_w;
            sprite_crop.y = 0 * sprite_h;
            sprite_crop.w = sprite_w;
            sprite_crop.h = sprite_h;


            break;

          case 1:
            // Visible Tile
            position.x = screen_pos.x;
            position.y = screen_pos.y;
            position.w = tile_w;
            position.h = tile_h;

            sprite_crop.x = 2 * sprite_w;
            sprite_crop.y = 0 * sprite_h;
            sprite_crop.w = sprite_w;
            sprite_crop.h = sprite_h;

            break;
          case 2:
            // Tree
            position.x = screen_pos.x;
            position.y = screen_pos.y - tile_h;
            position.w = tile_w;
            position.h = tile_h * 2;

            sprite_crop.x = 0 * sprite_w;
            sprite_crop.y = 1 * sprite_h;
            sprite_crop.w = sprite_w;
            sprite_crop.h = sprite_h * 2;

            break;

          case 3:
            // Spooky Tree
            position.x = screen_pos.x;
            position.y = screen_pos.y - tile_h;
            position.w = tile_w;
            position.h = tile_h * 2;

            sprite_crop.x = 1 * sprite_w;
            sprite_crop.y = 1 * sprite_h;
            sprite_crop.w = sprite_w;
            sprite_crop.h = sprite_h * 2;

            break;

          case 4:
            // Beach
            position.x = screen_pos.x;
            position.y = screen_pos.y;
            position.w = tile_w;
            position.h = tile_h;

            sprite_crop.x = 2 * sprite_w;
            sprite_crop.y = 2 * sprite_h;
            sprite_crop.w = sprite_w;
            sprite_crop.h = sprite_h;

            break;

          case 5:
            // Water
            position.x = screen_pos.x;
            position.y = screen_pos.y;
            position.w = tile_w;
            position.h = tile_h;

            sprite_crop.x = 3 * sprite_w;
            sprite_crop.y = 2 * sprite_h;
            sprite_crop.w = sprite_w;
            sprite_crop.h = sprite_h;

            break;
        }

        draw_texture(sprites, position, sprite_crop);
      }
    }

    // Draw coordinates
    const texture_t mouse_coord_text = create_text(
        "Mouse: " + STR(mouse_x) + ", " + STR(mouse_y), 0x000000FF, font);

    const texture_t cell_coord_text =
        create_text("Cell: " + STR(active_cell_x) + ", " + STR(active_cell_y),
                    0x000000FF, font);

    const texture_t tile_coord_text = create_text(
        "Tile: " + STR(selected_tile.x) + ", " + STR(selected_tile.y),
        0x000000FF, font);

    draw_texture(mouse_coord_text, 5, 5);
    draw_texture(cell_coord_text, 5, 20);
    draw_texture(tile_coord_text, 5, 35);
  }
};


int main()
{
  isometric p;

  if (p.run()) { return 0; }

  return 1;
}