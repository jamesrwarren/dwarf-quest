#pragma once

#include <SDL2/SDL.h>

#include "../components/transform.h"
#include "../components/combat.h"
#include "../components/health.h"

#include <entt/entt.hpp>

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

// Considers the following things and calculates total hitpoints remaining this frame:
// - damage taken this turn 
// - life gained this turn 
// - Any shields etc 
struct damage_system 
{  
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

    void render_cooldowns(entt::registry& registry, SDL_Renderer* renderer) 
    {
        auto view = registry.view<transform_component, combat_component, cooldown_component>();
        view.each([&](transform_component &transform, combat_component &damage, cooldown_component &cooldown) {
            
            
            float cooldown_percent = (damage.elapsed_time / (float)damage.strike_cooldown) * 100;

            // double cooldown_percent = 100;
            // Update the life bar's width based on the current hitpoints
            cooldown.bar_rect.w = (cooldown_percent * cooldown.width) / 100;  // Adjust according to max HP (100 in this example)

            cooldown.color = get_health_color(cooldown_percent);

            // std::cout << "ElapsedTime: " << damage.elapsed_time << " StrikeCD: " << damage.strike_cooldown << " CooldownPercent: " << cooldown_percent << '\n';

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Background Colour
            SDL_RenderFillRect(renderer, &cooldown.background_bar_rect);
            // Render the filled portion of the life bar
            SDL_SetRenderDrawColor(renderer, cooldown.color.r, cooldown.color.g, cooldown.color.b, 255); // Health color
            SDL_RenderFillRect(renderer, &cooldown.bar_rect);
        });
    }
};