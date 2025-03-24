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
                    if (damage_entity && damage_entity->apply_damage) {
                        hitpoints.damage_taken_this_turn += damage_entity->damage_per_hit;
                        damage_entity->apply_damage = false;
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

                if (weapon_owner_transform->direction == Direction::R) {
                    transform.pos_x += 25;
                    transform.direction = Direction::R;
                } else if (weapon_owner_transform->direction == Direction::L) {
                    transform.pos_x -= 25;
                    transform.direction = Direction::L;
                } else if (weapon_owner_transform->direction == Direction::U) {
                    transform.pos_y += 25;
                    transform.direction = Direction::U;
                } else if (weapon_owner_transform->direction == Direction::D) {
                    transform.pos_y -= 25;
                    transform.direction = Direction::D;
                } else if (weapon_owner_transform->direction == Direction::RU) {
                    transform.pos_x += 25;
                    transform.pos_y += 25;
                    transform.direction = Direction::RU;
                } else if (weapon_owner_transform->direction == Direction::RD) {
                    transform.pos_x += 25;
                    transform.pos_y -= 25;
                    transform.direction = Direction::RD;
                } else if (weapon_owner_transform->direction == Direction::LU) {
                    transform.pos_x -= 25;
                    transform.pos_y += 25;
                    transform.direction = Direction::LU;
                } else if (weapon_owner_transform->direction == Direction::LD) {
                    transform.pos_x -= 25;
                    transform.pos_y -= 25;
                    transform.direction = Direction::LD;
                } 
            } else {
                sprite.visible = false;
            } 
            sprite.dst.x = transform.pos_x;
            sprite.dst.y = transform.pos_y;
        });
    }  
};
