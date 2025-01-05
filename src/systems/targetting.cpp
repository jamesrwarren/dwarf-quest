#pragma once

#include <entt/entt.hpp>

#include "../components/player.h"
#include "../components/transform.h"
#include "../components/targetting.h"

struct targetting_system 
{
    void update(entt::registry& reg)
    {
        // Find the player by looking for the entity with a player_component
        transform_component* player_transform = nullptr;
        entt::entity player_entity = entt::null; // Initialize as null
        
        auto view_player = reg.view<transform_component, player_component>();
        view_player.each([&player_transform, &player_entity](entt::entity entity_player, transform_component &transform) {
            player_transform = &transform; 
            player_entity = entity_player;
        });

        // If the player is found
        if (player_transform) 
        {
            // Get a view of all enemies (entities with aquire_target_component)
            auto view_enemies = reg.view<transform_component, targetting_component>();

            // Update each enemy's velocity to move towards the player
            view_enemies.each([&](transform_component &enemy_transform, targetting_component &aquire_target) {
                aquire_target.target_entt = player_entity;
                aquire_target.player_x = player_transform->pos_x;
                aquire_target.player_y = player_transform->pos_y;
                aquire_target.target_x = aquire_target.player_x;
                aquire_target.target_y = aquire_target.player_y;

            });
        }
    }
};