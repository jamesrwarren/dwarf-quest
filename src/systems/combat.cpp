#pragma once

#include <iostream>

#include <entt/entt.hpp>

#include "../components/sprite.h"
#include "../components/transform.h"
#include "../components/collision.h"
#include "../components/health.h"
#include "../components/damage.h"
#include "../components/weapon.h"

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

            if (damage.attack_scheduled) {
                for (const entt::entity collided_entity : collision_detection.collided_entities) {
                    // Check if the collided entity has a hitpoints_component
                    if (!view_entities_with_hitpoints.contains(collided_entity)) { continue; }

                    hitpoints_component &target_hitpoints = view_entities_with_hitpoints.get<hitpoints_component>(collided_entity);
                    target_hitpoints.damage_taken_this_turn += damage.damage_per_hit;
                }
            }
        });
    }

    void update_weapon_states(entt::registry& reg)
    {
        Uint32 now = SDL_GetTicks();

        auto view_weapon_entities = reg.view<weapon_component, sprite_component, transform_component>();
        view_weapon_entities.each([&](entt::entity entity, weapon_component &weapon, sprite_component &sprite, transform_component &transform) {
            if (!reg.valid(weapon.owner_entt)) {
                return;
            }

            // Attempt to get all required components of the owner entity
            auto weapon_owner_damage = reg.try_get<damage_component>(weapon.owner_entt);
            
            

            // Ensure the weapon_owner_damage exists
            if (!weapon_owner_damage) {
                return;
            }

            auto weapon_owner_sprite = reg.try_get<sprite_component>(weapon.owner_entt);
            auto weapon_owner_transform = reg.try_get<transform_component>(weapon.owner_entt);

            Uint32 elapsed_time = now - weapon_owner_damage->last_strike;


            if (elapsed_time >= weapon_owner_damage->strike_cooldown) {
                if (weapon_owner_damage->attacking) {
                    weapon_owner_damage->attack_frames_remaining = weapon_owner_damage->attack_frames;
                    weapon_owner_damage->elapsed_time = weapon_owner_damage->strike_cooldown;
                    weapon_owner_damage->attack_scheduled = true;
                    weapon_owner_damage->last_strike = now;
                }            
            } else {
                weapon_owner_damage->attack_scheduled = false;
                weapon_owner_damage->elapsed_time = elapsed_time;
            }
            
            // std::cout << "ElapsedTime: " << weapon_owner_damage->elapsed_time << " LastStrike: " << weapon_owner_damage->last_strike << " StrikeCD: " << weapon_owner_damage->strike_cooldown <<  "\n";

            if (weapon_owner_damage->attack_frames_remaining > 0) { 

                weapon_owner_damage->attack_frames_remaining -= 1;

                if (weapon_owner_transform->direction == Direction::R) {
                    transform.pos_x += 25;
                } else if (weapon_owner_transform->direction == Direction::L) {
                    transform.pos_x -= 25;
                } else if (weapon_owner_transform->direction == Direction::U) {
                    transform.pos_y += 25;
                } else if (weapon_owner_transform->direction == Direction::D) {
                    transform.pos_y -= 25;
                } else if (weapon_owner_transform->direction == Direction::RU) {
                    transform.pos_x += 25;
                    transform.pos_y += 25;
                } else if (weapon_owner_transform->direction == Direction::RD) {
                    transform.pos_x += 25;
                    transform.pos_y -= 25;
                } else if (weapon_owner_transform->direction == Direction::LU) {
                    transform.pos_x -= 25;
                    transform.pos_y += 25;
                } else if (weapon_owner_transform->direction == Direction::LD) {
                    transform.pos_x -= 25;
                    transform.pos_y -= 25;
                } 
            }  
        });
    }
};