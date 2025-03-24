#pragma once

#include <any>
#include <vector>
#include <memory>
#include <cassert>
#include <iostream>
#include <queue>
#include <string>
#include <fstream> 
#include <algorithm>
#include <unordered_map>

#include <entt/entt.hpp>

#include "../components/sprite.h"
#include "../components/transform.h"
#include "../components/collidable.h"
#include "../components/weapon.h"
#include "../components/render_layer.h"
#include "../components/collision.h"

#include "collidable.cpp"

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};


using GridMap = std::unordered_map<std::pair<int, int>, std::vector<entt::entity>, pair_hash>;

template <typename ProcessEntityFunc>
void process_grid_cell(const GridMap& map, int grid_x, int grid_y, ProcessEntityFunc&& process_entity) {
    auto it = map.find({grid_x, grid_y});
    if (it != map.end()) {
        for (entt::entity entity : it->second) {
            process_entity(entity);
        }
    }
}

void check_collisions (
    collidable_system &s_collidable, 
    int &entity_proposed_x, 
    int &entity_proposed_y,
    const sprite_component &sprite_entity, 
    transform_component &transform_entity, 
    entt::entity &entt_target, 
    const sprite_component &sprite_target, 
    const collidable_component &collidable_target,
    bool &collision_detected, 
    bool &x_collision, 
    bool &y_collision, 
    bool &all_x_collisions, 
    bool &all_y_collisions, 
    collision_detection_component &collision_entity
) 
{
    std::cout << "Check Collison Between: (" << sprite_entity.label << " and " << sprite_target.label << ")" << "\n";
    std::cout << "SOURCE: (" << entity_proposed_x << "," << entity_proposed_y << ") - (" << sprite_entity.dst.w << "," << sprite_entity.dst.h << ") TARGET: (" << sprite_target.dst.x << "," << sprite_target.dst.y << ") - (" << sprite_target.dst.w << "," << sprite_target.dst.h << ")" << "\n";
    if (s_collidable.checkCollision(
        entity_proposed_x, entity_proposed_y, sprite_entity.dst.w, sprite_entity.dst.h, 
        sprite_target.dst.x, sprite_target.dst.y, sprite_target.dst.w, sprite_target.dst.h))
    {
        // std::cout << "Collison Between: (" << sprite_entity.label << " and " << sprite_target.label << ")" << "\n";
        // Add to colliding_entity collided list if visible
        if (sprite_target.visible) {
            collision_entity.collided_entities.push_back(entt_target);       
        }
        
        // Check separately for x and y collisions if moving diagonally
        if (transform_entity.vel_x != 0 && transform_entity.vel_y != 0) {
            x_collision = s_collidable.checkCollision(
                entity_proposed_x, transform_entity.pos_y, sprite_entity.dst.w, sprite_entity.dst.h, 
                sprite_target.dst.x, sprite_target.dst.y, sprite_target.dst.w, sprite_target.dst.h
            );
            y_collision = s_collidable.checkCollision(
                transform_entity.pos_x, entity_proposed_y, sprite_entity.dst.w, sprite_entity.dst.h, 
                sprite_target.dst.x, sprite_target.dst.y, sprite_target.dst.w, sprite_target.dst.h
            );
        }
        collision_detected = true;

        if (all_x_collisions || x_collision) {
            all_x_collisions = true;
        }

        if (all_y_collisions || y_collision) {
            all_y_collisions = true;
        }
    }
}

void process_by_grid_map (
    std::unordered_map<std::pair<int, int>, std::vector<entt::entity>, pair_hash> &grid_map,
    entt::registry& reg,
    int cell_x,
    int cell_y,
    collidable_system& s_collidable,
    int entity_proposed_x,
    int entity_proposed_y,
    const sprite_component& sprite_entity, 
    transform_component& transform_entity,
    bool& collision_detected,
    bool& x_collision,
    bool& y_collision,
    bool& all_x_collisions,
    bool& all_y_collisions,
    collision_detection_component& collision_entity,
    entt::entity entity,
    bool process_static
) {
    // TODO - add this to a global thing so we're not writing it on each loop
    auto view_all_collidables = reg.view<sprite_component, collidable_component>();

    process_grid_cell(grid_map, cell_x, cell_y, [&](entt::entity entt_target) {
        
        if (entity == entt_target) return; // Skip self-collision check
        
        const sprite_component& sprite_static_target = view_all_collidables.get<sprite_component>(entt_target);
        const collidable_component& collidable_static_target = view_all_collidables.get<collidable_component>(entt_target);

        // if (process_static) {
        //     std::cout << "STATIC Entity collidable: (" << sprite_static_target.grid_x << ", " << sprite_static_target.grid_y << ")" << "\n";
        // } else {
        //     std::cout << "DYNAMIC Entity collidable: (" << sprite_static_target.grid_x << ", " << sprite_static_target.grid_y << ")" << "\n";
        // }

        if (!process_static) {
            auto weapon_target = reg.try_get<weapon_component>(entt_target);
            if (weapon_target) {
                // Check if the owner of the weapon is the same as the current entity
                if (weapon_target->owner_entt == entity) {
                    return; // Skip self-collision check with own weapon
                }
            }
        }
        
        check_collisions(
            s_collidable, 
            entity_proposed_x, 
            entity_proposed_y, 
            sprite_entity, 
            transform_entity, 
            entt_target, 
            sprite_static_target,
            collidable_static_target,                            
            collision_detected, 
            x_collision, 
            y_collision, 
            all_x_collisions, 
            all_y_collisions, 
            collision_entity
        );
    });
}


struct collision_system 
{
    // Static grid map - populated once with static objects
    std::unordered_map<std::pair<int, int>, std::vector<entt::entity>, pair_hash> static_grid_map;

    // Function to initialize static grid map
    void load_static_entities(entt::registry& reg) {
        auto view_static = reg.view<sprite_component, collidable_component, background_component>();
        view_static.each([&](entt::entity entity, sprite_component &sprite, collidable_component &collidable) {
            std::pair<int, int> cell = { sprite.grid_x, sprite.grid_y };
            static_grid_map[cell].push_back(entity);
        });
    }

    void update(entt::registry& reg) {
        // std::cout << "No of Static Entities: " << std::size(static_grid_map) << "\n";
        collidable_system s_collidable;
        std::unordered_map<std::pair<int, int>, std::vector<entt::entity>, pair_hash> dynamic_grid_map;
        
        // Populate grid_map with dynamic entities
        auto view_dynamic_collidables = reg.view<sprite_component, transform_component, collidable_component>();
        view_dynamic_collidables.each([&](entt::entity entity, sprite_component& sprite, transform_component& transform, collidable_component &collidable) {
            dynamic_grid_map[{sprite.grid_x, sprite.grid_y}].push_back(entity);
        });

        // Loop through entities that can detect collisions (eg players, enemies etc...)
        // Bear in mind that weapons and explosions etc are collidable_components not collision_detection_components
        auto view_entity = reg.view<sprite_component, transform_component, collision_detection_component>();
        view_entity.each([&](entt::entity entity, sprite_component& sprite_entity, transform_component& transform_entity, collision_detection_component& collision_entity) {
            
            collision_entity.collided_entities.clear(); // Clear out collided stack each frame for each entity
            int entity_proposed_x = transform_entity.pos_x + transform_entity.vel_x;
            int entity_proposed_y = transform_entity.pos_y + transform_entity.vel_y;

            bool collision_detected = false;
            bool all_x_collisions = false, all_y_collisions = false;
            bool x_collision, y_collision = true;

            // Compute grid range based on entity size and speed
            const int grid_radius = 2;  // Example: Adjust based on entity speed and size
            for (int dx = -grid_radius; dx <= grid_radius; ++dx) {
                for (int dy = -grid_radius; dy <= grid_radius; ++dy) {
                    int cell_x = sprite_entity.grid_x + dx;
                    int cell_y = sprite_entity.grid_y + dy;

                    // ============= Process DYNAMIC entities =============
                    process_by_grid_map (
                        dynamic_grid_map,
                        reg,
                        cell_x,
                        cell_y,
                        s_collidable,
                        entity_proposed_x,
                        entity_proposed_y,
                        sprite_entity, 
                        transform_entity,
                        collision_detected,
                        x_collision,
                        y_collision,
                        all_x_collisions,
                        all_y_collisions,
                        collision_entity,
                        entity,
                        false
                    );

                    // ============= Process STATIC entities =============
                    process_by_grid_map (
                        static_grid_map,
                        reg,
                        cell_x,
                        cell_y,
                        s_collidable,
                        entity_proposed_x,
                        entity_proposed_y,
                        sprite_entity, 
                        transform_entity,
                        collision_detected,
                        x_collision,
                        y_collision,
                        all_x_collisions,
                        all_y_collisions,
                        collision_entity,
                        entity,
                        true
                    );
                }
            }

            // Adjust velocity if collision detected
            if (collision_detected) {
                if (all_x_collisions) { transform_entity.vel_x = 0; }
                if (all_y_collisions) { transform_entity.vel_y = 0; }
            }
        });
    }
};