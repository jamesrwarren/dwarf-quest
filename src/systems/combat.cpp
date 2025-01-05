#pragma once

#include <iostream>

#include <entt/entt.hpp>

#include "../components/sprite.h"
#include "../components/transform.h"
#include "../components/collision.h"
#include "../components/health.h"
#include "../components/damage.h"

struct combat_system 
{  
    void update(entt::registry& reg)
    {
        Uint32 now = SDL_GetTicks();

        // This view allows us to quickly check if an entity has a hitpoints_component
        auto view_entities_with_hitpoints = reg.view<hitpoints_component>();

        // Loop through Entities that can do damage!
        auto view_colliding_entities = reg.view<collision_detection_component, damage_component>();
        view_colliding_entities.each([&](collision_detection_component &collision_detection, damage_component &damage) {
            
            if (!damage.attacking) {
                return;
            }

            if (collision_detection.collided_entities.empty()) {
                return;
            }

            Uint32 elapsed_time = now - damage.last_strike;

            if (elapsed_time >= damage.strike_cooldown) {
                for (const entt::entity collided_entity : collision_detection.collided_entities) {
                    // Check if the collided entity has a hitpoints_component
                    if (!view_entities_with_hitpoints.contains(collided_entity)) { continue; }

                    hitpoints_component &target_hitpoints = view_entities_with_hitpoints.get<hitpoints_component>(collided_entity);
                    target_hitpoints.damage_taken_this_turn += damage.damage_per_hit;
                }

                damage.last_strike = now;
            }
        });
    }

    void update_striking_attack_boxes(entt::registry& reg)
    {
        Uint32 now = SDL_GetTicks();

        // Loop through Entities that can do damage!
        auto view_colliding_entities = reg.view<damage_component, transform_component, sprite_component>();
        view_colliding_entities.each([&](damage_component &damage, transform_component &transform, sprite_component &sprite) {
            
            if (!damage.attacking) {
                return;
            }

            Uint32 elapsed_time = now - damage.last_strike;

            if (elapsed_time >= damage.strike_cooldown) {
                if (transform.vel_x > 0) {
                    sprite.hit_box.w += 100;
                } else if (transform.vel_x < 0) {
                    sprite.hit_box.w += 100;
                    sprite.hit_box.x -= 100;
                    std::cout << "LEFT" << '\n';
                } else if (transform.vel_y > 0) {
                    sprite.hit_box.h += 100;
                    std::cout << "UP" << '\n';
                } else if (transform.vel_y < 0) {
                    sprite.hit_box.h += 100;
                    sprite.hit_box.y -= 100;
                    std::cout << "DOWN" << '\n';
                } 
            }
        });
    }

    void render_attack_box(entt::registry& registry, SDL_Renderer* renderer) 
    {
        auto view_colliding_entities = registry.view<damage_component, transform_component, sprite_component>();
        view_colliding_entities.each([&](damage_component &damage, transform_component &transform, sprite_component &sprite) {
            std::cout << "h: " << sprite.hit_box.h << '\n';
            std::cout << "w: " << sprite.hit_box.w << '\n';
            // Render the filled portion of the life bar
            if (damage.attacking) {
                SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255); // Health color
                SDL_RenderFillRect(renderer, &sprite.hit_box);
            }
            sprite.hit_box.x = sprite.dst.x;
            sprite.hit_box.y = sprite.dst.y;
            sprite.hit_box.h = sprite.dst.h;
            sprite.hit_box.w = sprite.dst.w;           
        });
    }
};