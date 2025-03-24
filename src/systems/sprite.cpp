#pragma once

#include "../components/transform.h"
#include "../components/sprite.h"
#include "../components/render_layer.h"
#include "../config/game_config.h"
#include "../components/weapon.h"

#include <entt/entt.hpp>

struct sprite_system 
{
    static std::pair<int, int> get_grid_position(int x, int y)
    {
        int grid_x = x / GameConfig::instance().grid_cell_width;
        int grid_y = y / GameConfig::instance().grid_cell_height;

        return std::make_pair(grid_x, grid_y);
    }

    void update_weapons(entt::registry& reg)
    {
        // Updates position (will not pull back terrain as terrain has no transform component)
        auto view_weapon = reg.view<sprite_component, weapon_component>();
        view_weapon.each([&](entt::entity entity, sprite_component &sprite, weapon_component &weapon){

            auto weapon_owner_transform = reg.try_get<transform_component>(weapon.owner_entt);

            sprite.dst.x = weapon_owner_transform->pos_x;
            sprite.dst.y = weapon_owner_transform->pos_y;

            auto [grid_x, grid_y] = get_grid_position(sprite.dst.x, sprite.dst.y);
            sprite.grid_x = grid_x;
            sprite.grid_y = grid_y;           
        });
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

    void render_background(entt::registry& reg, SDL_Renderer* renderer)
    {
        // Create a view for sprite components
        auto view_sprite = reg.view<sprite_component, background_component>();

        view_sprite.each([&](sprite_component &sprite) {

            SDL_RenderCopy(
                renderer, 
                sprite.texture, 
                &sprite.src, 
                &sprite.dst
            );
        });
    }

    void render_layer_one(entt::registry& reg, SDL_Renderer* renderer)
    {
        // Create a view for sprite components
        auto view_sprite = reg.view<sprite_component, transform_component, layer_one_component>();

        view_sprite.each([&](sprite_component &sprite, transform_component &transform) {
            if (!sprite.visible) {
                return;
            }
            double angle = 0;
            if (transform.direction == Direction::R) {
                angle = 90.0;
            } else if (transform.direction == Direction::L) {
                angle = 270.0;
            } else if (transform.direction == Direction::U) {
                angle = 180.0;
            } else if (transform.direction == Direction::D) {
                angle = 0.0;
            } else if (transform.direction == Direction::RU) {
                angle = 135.0;
            } else if (transform.direction == Direction::RD) {
                angle = 45.0;
            } else if (transform.direction == Direction::LU) {
                angle = 225.0;
            } else if (transform.direction == Direction::LD) {
                angle = 315.0;
            } 

            SDL_RenderCopyEx(renderer, sprite.texture, &sprite.src, &sprite.dst, angle, nullptr, SDL_FLIP_NONE);

            // SDL_RenderCopy(
            //     renderer, 
            //     sprite.texture, 
            //     &sprite.src, 
            //     &sprite.dst
            // );
        });
    }

    void render_layer_two(entt::registry& reg, SDL_Renderer* renderer)
    {
        // Create a view for sprite components
        auto view_sprite = reg.view<sprite_component, layer_two_component>();

        view_sprite.each([&](sprite_component &sprite) {
            SDL_RenderCopy(
                renderer, 
                sprite.texture, 
                &sprite.src, 
                &sprite.dst
            );
        });
    }
};