#pragma once

#include <iostream>

#include "../../components/sprite.h"
#include "../../components/transform.h"
#include "../../components/collision.h"
#include "../../components/health.h"
#include "../../components/player.h"
#include "../../components/path_finding.h"
#include "../../components/targetting.h"
#include "../../components/render_layer.h"

#include <entt/entt.hpp>


struct logging_system 
{
    Uint32 last_print_time = SDL_GetTicks();
    
    void update(entt::registry& reg, int log_interval)
    {
        Uint32 now = SDL_GetTicks();
        Uint32 elapsed_time = now - last_print_time;
        
        if (elapsed_time >= (log_interval * 1000)) {
            // Players State
            auto view_players = reg.view<sprite_component, transform_component, collision_detection_component, hitpoints_component, player_component>();
            view_players.each([&](entt::entity player_entity, sprite_component &player_sprite, transform_component &player_transform, collision_detection_component &player_collidable, hitpoints_component &player_hitpoints) {
                std::cout << "Player ID: " << static_cast<uint32_t>(player_entity) 
                << " -- X,Y: (" << player_transform.pos_x << "," << player_transform.pos_y
                << ") -- Grid X,Y: (" << player_sprite.grid_x << "," << player_sprite.grid_y << ")" << '\n';

                for (const auto& collided_entity : player_collidable.collided_entities) {
                    std::cout << "Player Collided With: " << static_cast<uint32_t>(collided_entity) << '\n';
                }

                std::cout << "Player Hitpoints: " << player_hitpoints.hitpoints << '\n';
            });
        
            // Enemies State
            auto view_enemies = reg.view<sprite_component, transform_component, path_finding_component, targetting_component, hitpoints_component>();
            view_enemies.each([&](entt::entity enemy_entity, sprite_component &enemy_sprite, transform_component &enemy_transform, path_finding_component &enemy_path_finding, 
            targetting_component &enemy_targetting, hitpoints_component &enemy_hitpoints) 
            {
                std::cout << "Enemy ID: " << static_cast<uint32_t>(enemy_entity) << '\n'; 
                std::cout << "Path from (" << enemy_sprite.grid_x << ", " << enemy_sprite.grid_y << ")" << " of size " << std::size(enemy_path_finding.path) << "\n";
                
                for (const auto& node : enemy_path_finding.path) {
                    std::cout << "Node at (" << node.grid_x << ", " << node.grid_y << ") with f-cost: " << node.f_cost() << '\n';
                }

                std::cout << "Enemy Hitpoints: " << enemy_hitpoints.hitpoints << '\n';
            });

            auto view_static = reg.view<sprite_component, transform_component, background_component>();
            view_static.each([&](entt::entity static_entity, sprite_component &static_sprite, transform_component &static_transform) 
            {
                std::cout << "Static ID: " << static_cast<uint32_t>(static_entity) << "Path from (" << static_sprite.grid_x << ", " << static_sprite.grid_y << ") xy_pos: " << static_sprite.dst.x << ", " << static_sprite.dst.y << "\n";
            });

            // std::cout << entity_proposed_x << "," << transform_entity.pos_y << '\n';
            // std::cout << transform_entity.pos_x << "," << entity_proposed_y << '\n';
            // std::cout << sprite_collidable.dst.x << "," << sprite_collidable.dst.x + sprite_collidable.dst.w << "," << sprite_collidable.dst.y << "," << sprite_collidable.dst.y + sprite_collidable.dst.h << '\n';

            last_print_time = now;
        
        }
    }
};