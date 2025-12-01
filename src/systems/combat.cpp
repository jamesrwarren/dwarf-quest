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
        auto view_character_entities = reg.view<collision_detection_component, hitpoints_component, transform_component>();
        view_character_entities.each([&](collision_detection_component &collision_detection, hitpoints_component &hitpoints, transform_component &transform)
        {

            if (collision_detection.collided_entities.empty()) {
                return;
            }

            // loop through the collided entities and check for damaging entities then work out how much damage they do
            for (const entt::entity collided_entity : collision_detection.collided_entities) {

                    // Only do something if character has collided with a thing that does damage
                    if (!view_damaging_entities.contains(collided_entity)) { continue; }

                    auto damage_entity = reg.try_get<damage_component>(collided_entity);
                    
                    // Only do something if it's not a friendly type
                    if (damage_entity->type == collision_detection.type) { continue; }
                    
                    if (damage_entity && damage_entity->apply_damage) {
                        hitpoints.damage_taken_this_turn += damage_entity->damage_per_hit;
                        damage_entity->apply_damage = false;
                        if (damage_entity->stun) {
                            if (!hitpoints.stunned) {
                                hitpoints.stunned = true;
                                hitpoints.stunned_frames_remaining = hitpoints.stunned_frames;
                            }
                        }
                    }
                }
            }
        );        
    }

    void update_character_statuses(entt::registry& reg) {
        auto view_character_entities = reg.view<hitpoints_component, transform_component>();
        view_character_entities.each([&](hitpoints_component &hitpoints, transform_component &transform)
        {
            if (hitpoints.stunned) {
                if (hitpoints.stunned_frames_remaining > 0) {
                    hitpoints.stunned_frames_remaining -= 1;
                    // std::cout << "STUNNED: " << hitpoints.stunned_frames_remaining << "\n";
                    transform.vel_x = 0;
                    transform.vel_y = 0;
                } else {
                    hitpoints.stunned = false;
                }
            }
        });
    }

    void update_weapon_states(entt::registry& reg)
    {
        Uint32 now = SDL_GetTicks();

        auto view_weapon_entities = reg.view<weapon_component, sprite_component, transform_component, damage_component>();
        view_weapon_entities.each([&](entt::entity entity, weapon_component &weapon, sprite_component &sprite, transform_component &transform, damage_component &damage) {
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
                    damage.apply_damage = true;
                }            
            } else {
                weapon_owner_combat->attack_scheduled = false;
                weapon_owner_combat->elapsed_time = elapsed_time;
            }
            
            // std::cout << "ElapsedTime: " << weapon_owner_damage->elapsed_time << " LastStrike: " << weapon_owner_damage->last_strike << " StrikeCD: " << weapon_owner_damage->strike_cooldown <<  "\n";

            if (weapon_owner_combat->attack_frames_remaining > 0) {
                sprite.visible = true;
                weapon_owner_combat->attack_frames_remaining -= 1;

                const Direction dir = weapon_owner_transform->direction;
                transform.direction = dir;

                int dx = 0;
                int dy = 0;

                switch (dir) {
                    case Direction::R:  dx = +25; break;
                    case Direction::L:  dx = -25; break;
                    case Direction::U:  dy = +25; break;
                    case Direction::D:  dy = -25; break;
                    case Direction::RU: dx = +25; dy = +25; break;
                    case Direction::RD: dx = +25; dy = -25; break;
                    case Direction::LU: dx = -25; dy = +25; break;
                    case Direction::LD: dx = -25; dy = -25; break;
                    default: break;
                }

                transform.pos_x += dx;
                transform.pos_y += dy;
            } else {
                sprite.visible = false;
            }
            sprite.dst.x = transform.pos_x;
            sprite.dst.y = transform.pos_y;
        });
    }  
};
