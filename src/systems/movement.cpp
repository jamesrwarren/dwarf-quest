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
            if (keys[SDL_SCANCODE_A]) { transform.vel_x = -transform.speed; } 
            if (keys[SDL_SCANCODE_S]) { transform.vel_y = transform.speed; }
            if (keys[SDL_SCANCODE_W]) { transform.vel_y = -transform.speed; }
            if (keys[SDL_SCANCODE_D]) { transform.vel_x = transform.speed; }
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
        view_transform.each([](transform_component& transform) {

            const int vx = transform.vel_x;
            const int vy = transform.vel_y;

            if (vx == 0 && vy == 0) {
                return; // No movement, keep current direction
            }

            const bool moving_right = vx > 0;
            const bool moving_left  = vx < 0;
            const bool moving_up    = vy > 0;
            const bool moving_down  = vy < 0;

            if (moving_right && !moving_up && !moving_down) {
                transform.direction = Direction::R;
            } else if (moving_right && moving_up) {
                transform.direction = Direction::RU;
            } else if (moving_right && moving_down) {
                transform.direction = Direction::RD;
            } else if (moving_left && !moving_up && !moving_down) {
                transform.direction = Direction::L;
            } else if (moving_left && moving_up) {
                transform.direction = Direction::LU;
            } else if (moving_left && moving_down) {
                transform.direction = Direction::LD;
            } else if (!moving_left && !moving_right && moving_up) {
                transform.direction = Direction::U;
            } else if (!moving_left && !moving_right && moving_down) {
                transform.direction = Direction::D;
            }
        });
    }
};
