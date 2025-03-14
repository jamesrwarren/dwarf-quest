#pragma once

#include <iostream>

#include <entt/entt.hpp>

#include "../components/sprite.h"
#include "../components/transform.h"
#include "../components/collision.h"
#include "../components/health.h"
#include "../components/combat.h"
#include "../components/damage.h"
#include "../components/weapon.h"

struct combat_system 
{  
    // Rewrite this
    void update(entt::registry& reg)
    {
        auto view_damaging_entities = reg.view<damage_component>();

        // Loop through Entities that can do damage!
        auto view_character_entities = reg.view<collision_detection_component, hitpoints_component>();
        view_character_entities.each([&](collision_detection_component &collision_detection, hitpoints_component &hitpoints) {

            if (collision_detection.collided_entities.empty()) {
                return;
            }

            // loop through the collided entities and check for damaging entities then work out how much damage they do
            for (const entt::entity collided_entity : collision_detection.collided_entities) {
                    
                    // Only do something if character has collided with a thing that does damage
                    if (!view_damaging_entities.contains(collided_entity)) { continue; }

                    auto damage_entity = reg.try_get<damage_component>(collided_entity);
                    if (damage_entity) {
                        hitpoints.damage_taken_this_turn += damage_entity->damage_per_hit;
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
            auto weapon_owner_combat = reg.try_get<combat_component>(weapon.owner_entt);
            // Ensure the weapon_owner_damage exists
            if (!weapon_owner_combat) {
                return;
            }

            auto weapon_owner_sprite = reg.try_get<sprite_component>(weapon.owner_entt);
            auto weapon_owner_transform = reg.try_get<transform_component>(weapon.owner_entt);

            Uint32 elapsed_time = now - weapon_owner_combat->last_strike;

            if (elapsed_time >= weapon_owner_combat->strike_cooldown) {
                if (weapon_owner_combat->attacking) {
                    weapon_owner_combat->attack_frames_remaining = weapon_owner_combat->attack_frames;
                    weapon_owner_combat->elapsed_time = weapon_owner_combat->strike_cooldown;
                    weapon_owner_combat->attack_scheduled = true;
                    weapon_owner_combat->last_strike = now;
                }            
            } else {
                weapon_owner_combat->attack_scheduled = false;
                weapon_owner_combat->elapsed_time = elapsed_time;
            }
            
            // std::cout << "ElapsedTime: " << weapon_owner_damage->elapsed_time << " LastStrike: " << weapon_owner_damage->last_strike << " StrikeCD: " << weapon_owner_damage->strike_cooldown <<  "\n";

            if (weapon_owner_combat->attack_frames_remaining > 0) { 
                sprite.visible = true;
                weapon_owner_combat->attack_frames_remaining -= 1;

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
            } else {
                sprite.visible = false;
            } 
        });
    }
};


// Uint32 now = SDL_GetTicks();

        // // This view allows us to quickly check if an entity has a hitpoints_component
        // auto view_entities_with_hitpoints = reg.view<hitpoints_component>();
        // auto view_weapons_entities = reg.view<weapon_component>();
        // auto view_characters = reg.view<weapon_component>();

        // // Loop through Entities that can do damage!
        // auto view_colliding_entities = reg.view<collision_detection_component, combat_component>();
        // view_colliding_entities.each([&](collision_detection_component &collision_detection, combat_component &damage) {
            
        //     if (!damage.attacking) {
        //         return;
        //     }

        //     if (collision_detection.collided_entities.empty()) {
        //         return;
        //     }

        //     // loop through the collided entities and check for weapons then work out how much damage they do
        //     for (const entt::entity collided_entity : collision_detection.collided_entities) {
        //             // Only do something if character has collided with a weapon
        //             if (!view_weapons_entities.contains(collided_entity)) { continue; }

        //             auto weapon_target = reg.try_get<weapon_component>(collided_entity);
        //             if (weapon_target) {
        //                 // Get the owner of the weapon
        //             }

        //             hitpoints_component &target_hitpoints = view_entities_with_hitpoints.get<hitpoints_component>(collided_entity);
        //             target_hitpoints.damage_taken_this_turn += combat.damage_per_hit;
        //         }


        //     if (damage.attack_scheduled) {
        //         for (const entt::entity collided_entity : collision_detection.collided_entities) {
        //             // Check if the collided entity has a hitpoints_component
        //             if (!view_entities_with_hitpoints.contains(collided_entity)) { continue; }

        //             hitpoints_component &target_hitpoints = view_entities_with_hitpoints.get<hitpoints_component>(collided_entity);
        //             target_hitpoints.damage_taken_this_turn += damage.damage_per_hit;
        //         }
        //     }
        // });