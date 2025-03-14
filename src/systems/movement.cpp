#pragma once

#include <SDL2/SDL.h>

#include "../components/transform.h"
#include "../components/player.h"
#include "../components/combat.h"
#include "../components/targetting.h"

#include <entt/entt.hpp>

struct movement_system 
{  
    void update_players(entt::registry& reg)
    {
        const Uint8* keys = SDL_GetKeyboardState(NULL);

        // Update movement based on key inputs
        auto view_player = reg.view<transform_component, combat_component, player_component>();
        view_player.each([&keys, &reg](transform_component &transform, combat_component &combat){
            // Apply movement based on input
            if (keys[SDL_SCANCODE_A]) { transform.vel_x = -1; } 
            if (keys[SDL_SCANCODE_S]) { transform.vel_y = 1; }
            if (keys[SDL_SCANCODE_W]) { transform.vel_y = -1; }
            if (keys[SDL_SCANCODE_D]) { transform.vel_x = 1; }
            if (keys[SDL_SCANCODE_L]) { combat.attacking = true; }
            if (!keys[SDL_SCANCODE_L]) { combat.attacking = false; }
            if (!keys[SDL_SCANCODE_A] && !keys[SDL_SCANCODE_D]) { transform.vel_x = 0; }
            if (!keys[SDL_SCANCODE_S] && !keys[SDL_SCANCODE_W]) { transform.vel_y = 0; }    
        });
    }

    void update_enemies(entt::registry& reg)
    {
        // Get a view of all enemies (entities with targetting_component)
        auto view_enemies = reg.view<transform_component, targetting_component>();
        view_enemies.each([&](transform_component &enemy_transform, targetting_component &aquire_target) {

            // Calculate direction towards player
            int direction_x = aquire_target.target_x - enemy_transform.pos_x;
            int direction_y = aquire_target.target_y - enemy_transform.pos_y;

            // Set velocity to move towards the player using sign-based normalization
            enemy_transform.vel_x = (direction_x > 0) - (direction_x < 0); // -1, 0, or +1 based on the direction
            enemy_transform.vel_y = (direction_y > 0) - (direction_y < 0); // -1, 0, or +1 based on the direction
        });
    }

    void update_directions(entt::registry& reg)
    {
        auto view_transform = reg.view<transform_component>();
        view_transform.each([&](transform_component &transform) {

            if (transform.vel_x > 0 && transform.vel_y == 0) {
                transform.direction = Direction::R;
            } else if (transform.vel_x > 0 && transform.vel_y > 0) {
                transform.direction = Direction::RU;
            } else if (transform.vel_x < 0 && transform.vel_y == 0) {
                transform.direction = Direction::L;
            } else if (transform.vel_x < 0 && transform.vel_y > 0) {
                transform.direction = Direction::LU;
            } else if (transform.vel_y > 0 && transform.vel_x == 0) {
                transform.direction = Direction::U;
            } else if (transform.vel_y < 0 && transform.vel_x == 0) {
                transform.direction = Direction::D;
            } else if (transform.vel_x < 0 && transform.vel_y < 0) {
                transform.direction = Direction::LD;
            } else if (transform.vel_x > 0 && transform.vel_y < 0) {
                transform.direction = Direction::RD;
            }
        });
    }
};
