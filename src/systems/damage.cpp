#pragma once

#include <SDL2/SDL.h>

#include "../components/transform.h"
#include "../components/damage.h"
#include "../components/health.h"

#include <entt/entt.hpp>

struct damage_system 
{  
    SDL_Color get_health_color(int health) 
    {
        if (health > 60) {
            return {0, 255, 0, 255}; // Green for full health
        } else if (health > 30) {
            return {255, 255, 0, 255}; // Yellow for medium health
        } else {
            return {255, 0, 0, 255}; // Red for low health
        }
    }

    void update(entt::registry& reg)
    {
        auto view_take_damage = reg.view<hitpoints_component, life_bar_component>();
        view_take_damage.each([&](hitpoints_component &hitpoints, life_bar_component &life_bar){
            if (hitpoints.damage_taken_this_turn > 0) {
                hitpoints.hitpoints -= hitpoints.damage_taken_this_turn;
                hitpoints.damage_taken_this_turn = 0;
            }
            if (hitpoints.hitpoints < 0) hitpoints.hitpoints = 0;
            
        });
    }

    void render_life_bars(entt::registry& registry, SDL_Renderer* renderer) 
    {
        auto view = registry.view<transform_component, hitpoints_component, life_bar_component>();
        view.each([&](transform_component &transform, hitpoints_component &hitpoints, life_bar_component &life_bar) {
            
            
            // Position the life bar above the entity

            life_bar.bar_rect.x = transform.pos_x;
            life_bar.bar_rect.y = transform.pos_y - life_bar.height - 5;  // Position above the entity

            double hitpoints_percent = (double)hitpoints.hitpoints / hitpoints.full_health_hitpoints * 100;
            // Update the life bar's width based on the current hitpoints
            life_bar.bar_rect.w = (hitpoints_percent * life_bar.width) / 100;  // Adjust according to max HP (100 in this example)

            life_bar.color = get_health_color(hitpoints_percent);

            // Render the filled portion of the life bar
            SDL_SetRenderDrawColor(renderer, life_bar.color.r, life_bar.color.g, life_bar.color.b, 255); // Health color
            SDL_RenderFillRect(renderer, &life_bar.bar_rect);
        });
    }
};