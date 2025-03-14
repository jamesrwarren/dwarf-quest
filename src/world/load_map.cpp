#pragma once

#include <iostream>

#include <entt/entt.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../components/transform.h"
#include "../components/sprite.h"
#include "../components/render_layer.h"
#include "../components/collidable.h"
#include "../systems/sprite.cpp"

#include "../config/game_config.h"


void load_map(const std::string& filename, entt::registry& registry, SDL_Renderer* renderer)
{   
    std::ifstream map_file(filename);
    if (!map_file.is_open()) {
        std::cerr << "Error: Could not open map file " << filename << '\n';
        return;
    }
    const int tile_width = GameConfig::instance().grid_cell_width;
    const int tile_height = GameConfig::instance().grid_cell_height;
    int row = 0;

    sprite_system sprite_system_inst;

    std::string line;
    while (std::getline(map_file, line)) {
        for (int col = 0; col < line.size(); ++col) {
            char tile = line[col];
            if (tile == 'd' || tile == 'g') {
                // Create entity for this tile
                entt::entity entity = registry.create();

                // Assign sprite component
                sprite_component& sprite = registry.emplace<sprite_component>(entity);
                registry.emplace<background_component>(entity);

                sprite.visible = true;
                sprite.label = std::string("TERRAIN");
                // The image being used for the sprite
                sprite.src.x = 0;
                sprite.src.y = 0;
                sprite.src.w = 200;
                sprite.src.h = 200;

                sprite.dst.x = col * tile_width;
                sprite.dst.y = row * tile_height;
                sprite.dst.w = tile_width;
                sprite.dst.h = tile_height;

                auto [grid_x, grid_y] = sprite_system_inst.get_grid_position(sprite.dst.x, sprite.dst.y);
                sprite.grid_x = grid_x;
                sprite.grid_y = grid_y;

                // Load texture based on tile type
                if (tile == 'd') {
                    sprite.texture = IMG_LoadTexture(renderer, "assets/images/dirt.jpeg");
                    registry.emplace<collidable_component>(entity, true);
                    std::cout << "Loaded DIRT" << std::endl;
                } else if (tile == 'g') {
                    sprite.texture = IMG_LoadTexture(renderer, "assets/images/grass.png");
                    std::cout << "Loaded GRASS" << std::endl;
                }

                if (!sprite.texture) {
                    std::cerr << "Error loading texture for tile: " << SDL_GetError() << '\n';
                }
            }
        }
        ++row;
    }
}