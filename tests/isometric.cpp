#include <math.h>
#include <algorithm>
#include <vector>
#include "pixello.hpp"

constexpr int screen_w = 800;
constexpr int screen_h = 400;

// Number of tiles in world
constexpr int world_w = 20;
constexpr int world_h = 10;

// Size of single tile graphic
constexpr int tile_w = 40;
constexpr int tile_h = 20;

// Where to place tile (0,0) on screen (in tile size steps)
constexpr float scroll_speed = 0.0000007f;
point_t map_render_offset = {200, 200};

// Sprite that holds all imagery
constexpr int sprite_w = 40;
constexpr int sprite_h = 20;
texture_t sprites;
font_t font;

// Pointer to create 2D world array
int world[world_h][world_w];


point_t coord_screen_to_map(const point_t screen_coord)
{
  point_t tile_coord;

  float sx, sy;

  sx = screen_coord.x - (tile_w / 2.0f);
  sy = screen_coord.y - (tile_h / 2.0f);

  sx -= map_render_offset.x;
  sy -= map_render_offset.y;

  tile_coord.x = static_cast<int>(roundf(((sx / tile_w) - (sy / tile_h))));
  tile_coord.y = static_cast<int>(roundf(((sx / tile_w) + (sy / tile_h))));

  return tile_coord;
}


point_t coord_map_to_screen(const point_t& map_cord)
{
  point_t iso_coord;

  iso_coord.x = map_render_offset.x +
                ((map_cord.x * tile_w / 2) + (map_cord.y * tile_w / 2));

  iso_coord.y = map_render_offset.y +
                ((map_cord.y * tile_h / 2) - (map_cord.x * tile_h / 2));

  return iso_coord;
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
        element = 0;
      }
      ++i;
    }
    world[1][0] = 1;
    world[2][0] = 2;
    world[3][0] = 3;
  }

  void on_update(void*) override
  {
    {  // Useless
      if (mouse_state().left_button.click) { log("Click!"); }

      // Check if we have to quit the game
      if (is_key_pressed(keycap_t::ESC)) { stop(); }

      if (is_key_pressed(keycap_t::LEFT)) {
        map_render_offset.x -=
            static_cast<int>(floorf(scroll_speed * delta_time()));
      }
      if (is_key_pressed(keycap_t::RIGHT)) {
        map_render_offset.x +=
            static_cast<int>(floorf(scroll_speed * delta_time()));
      }
      if (is_key_pressed(keycap_t::UP)) {
        map_render_offset.y -=
            static_cast<int>(floorf(scroll_speed * delta_time()));
      }
      if (is_key_pressed(keycap_t::DOWN)) {
        map_render_offset.y +=
            static_cast<int>(floorf(scroll_speed * delta_time()));
      }
    }

    // Clear screen
    draw_rect({0, 0, screen_w, screen_h}, 0xFFFFFFFF);

    // Draw in this order to draw first the objects that are more far away
    for (int x = (world_w - 1); x > -1; --x) {
      for (int y = 0; y < world_h; ++y) {
        const point_t screen_pos = coord_map_to_screen({x, y});

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


    // Selected tile
    const int mouse_x = mouse_state().x;
    const int mouse_y = mouse_state().y;

    const point_t selected_tile = coord_screen_to_map({mouse_x, mouse_y});
    const point_t screen_selected_tile = coord_map_to_screen(selected_tile);

    rect_t position;
    rect_t sprite_crop;
    position.x = screen_selected_tile.x;
    position.y = screen_selected_tile.y;
    position.w = tile_w;
    position.h = tile_h;

    sprite_crop.x = 0 * sprite_w;
    sprite_crop.y = 0 * sprite_h;
    sprite_crop.w = sprite_w;
    sprite_crop.h = sprite_h;

    draw_texture(sprites, position, sprite_crop);

    // Draw coordinates
    const texture_t mouse_coord_text = create_text(
        "Mouse: " + STR(mouse_x) + ", " + STR(mouse_y), 0x000000FF, font);

    const texture_t tile_coord_text = create_text(
        "Tile: " + STR(selected_tile.x) + ", " + STR(selected_tile.y),
        0x000000FF, font);

    draw_texture(mouse_coord_text, 5, 5);
    draw_texture(tile_coord_text, 5, 20);

    // Change the tile
    if (mouse_state().left_button.click) {
      if (selected_tile.x < world_w && selected_tile.x >= 0 &&
          selected_tile.y < world_h && selected_tile.y >= 0) {
        const int current_val = world[selected_tile.y][selected_tile.x];
        world[selected_tile.y][selected_tile.x] = (current_val + 1) % 6;
      }
    }
  }
};


int main()
{
  isometric p;

  if (p.run()) { return 0; }

  return 1;
}