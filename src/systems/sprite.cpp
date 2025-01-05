#pragma once

#include "../components/transform.h"
#include "../components/sprite.h"
#include "../config/game_config.h"
#include <entt/entt.hpp>

struct sprite_system 
{
    static std::pair<int, int> get_grid_position(int x, int y)
    {
        int grid_x = x / GameConfig::instance().grid_cell_width;
        int grid_y = y / GameConfig::instance().grid_cell_height;

        return std::make_pair(grid_x, grid_y);
    }

    void update(entt::registry& reg)
    {
        // Updates position (will not pull back terrain as terrain has no transform component)
        auto view_transform = reg.view<sprite_component, transform_component>();
        view_transform.each([&](entt::entity entity, sprite_component &sprite, transform_component &transform){
                sprite.dst.x = transform.pos_x;
                sprite.dst.y = transform.pos_y;

                auto [grid_x, grid_y] = get_grid_position(sprite.dst.x, sprite.dst.y);
                sprite.grid_x = grid_x;
                sprite.grid_y = grid_y;           
        });
    }

    void render(entt::registry& reg, SDL_Renderer* renderer)
    {
        // Create a view for sprite components
        auto view_sprite = reg.view<sprite_component>();

        // Use reverse iterators to render in ascending order of entity IDs
        for (auto this_sprite = view_sprite.rbegin(); this_sprite != view_sprite.rend(); ++this_sprite) {
            entt::entity entity = *this_sprite;
            
            sprite_component& sprite = view_sprite.get<sprite_component>(entity);
            // Render the sprite using SDL_RenderCopy
            SDL_RenderCopy(
                renderer, 
                sprite.texture, 
                &sprite.src, 
                &sprite.dst
            );
        }
    }
};