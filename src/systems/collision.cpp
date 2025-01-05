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
#include "../components/static.h"
#include "../components/collision.h"

#include "collidable.cpp"

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

struct collision_system 
{
    // Static grid map - populated once with static objects
    std::unordered_map<std::pair<int, int>, std::vector<entt::entity>, pair_hash> static_grid_map;

    // Function to initialize static grid map
    void load_static_entities(entt::registry& reg) {
        auto view_static = reg.view<sprite_component, collidable_component, static_component>();
        view_static.each([&](entt::entity entity, sprite_component &sprite) {
            std::pair<int, int> cell = { sprite.grid_x, sprite.grid_y };
            static_grid_map[cell].push_back(entity);
        });
    }

    void check_collisions (
        collidable_system &s_collidable, int &entity_proposed_x, int &entity_proposed_y,
        sprite_component &sprite_entity, entt::entity &entity_collidable, const sprite_component &sprite_collidable, transform_component &transform_entity, 
        bool &collision_detected, bool &x_collision, bool &y_collision, bool &all_x_collisions, bool &all_y_collisions, 
        collision_detection_component &collision_entity
    ) 
    {
        if (s_collidable.checkCollision(
            entity_proposed_x, entity_proposed_y, sprite_entity.dst.w, sprite_entity.dst.h, 
            sprite_collidable.dst.x, sprite_collidable.dst.y, sprite_collidable.dst.w, sprite_collidable.dst.h))
        {
            // Add to colliding_entity collided list
            collision_entity.collided_entities.push_back(entity_collidable);
            
            // Check separately for x and y collisions if moving diagonally
            if (transform_entity.vel_x != 0 && transform_entity.vel_y != 0) {
                x_collision = s_collidable.checkCollision(
                    entity_proposed_x, transform_entity.pos_y, sprite_entity.dst.w, sprite_entity.dst.h, 
                    sprite_collidable.dst.x, sprite_collidable.dst.y, sprite_collidable.dst.w, sprite_collidable.dst.h
                );
                y_collision = s_collidable.checkCollision(
                    transform_entity.pos_x, entity_proposed_y, sprite_entity.dst.w, sprite_entity.dst.h, 
                    sprite_collidable.dst.x, sprite_collidable.dst.y, sprite_collidable.dst.w, sprite_collidable.dst.h
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

    void update(entt::registry& reg) {
        collidable_system s_collidable;
        std::unordered_map<std::pair<int, int>, std::vector<entt::entity>, pair_hash> grid_map;
        
        auto view_all_collidables = reg.view<sprite_component, collidable_component>();
        auto view_non_static_collidables = reg.view<sprite_component, transform_component, collidable_component>();

        // Populate grid_map with dynamic entities
        view_non_static_collidables.each([&](entt::entity entity, sprite_component& sprite, transform_component& transform) {
            grid_map[{sprite.grid_x, sprite.grid_y}].push_back(entity);
        });

        // Helper lambda for processing entities in a grid cell
        auto process_cell = [&](const std::unordered_map<std::pair<int, int>, std::vector<entt::entity>, pair_hash>& map, 
                                int grid_x, int grid_y, 
                                auto&& process_entity) {
            auto it = map.find({grid_x, grid_y});
            if (it != map.end()) {
                for (entt::entity entity : it->second) {
                    process_entity(entity);
                }
            }
        };

        // Loop through entities that can detect collisions (eg players, enemies etc...)
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

                    // Process dynamic entities
                    process_cell(grid_map, cell_x, cell_y, [&](entt::entity entity_collidable) {
                        if (entity == entity_collidable) return; // Skip self-collision check
                        
                        const sprite_component& sprite_collidable = view_all_collidables.get<sprite_component>(entity_collidable);
                        // std::cout << "Entity collidable: (" << sprite_collidable.grid_x << ", " << sprite_collidable.grid_y << ")" << " Dynamic Entity: " << static_cast<uint32_t>(entity_collidable) << "\n";
                        check_collisions(
                            s_collidable, entity_proposed_x, entity_proposed_y, sprite_entity, entity_collidable, sprite_collidable,
                            transform_entity, collision_detected, x_collision, y_collision, all_x_collisions, all_y_collisions, collision_entity
                        );
                    });

                    // Process static entities
                    process_cell(static_grid_map, cell_x, cell_y, [&](entt::entity entity_static_collidable) {
                        const sprite_component& sprite_static_collidable = view_all_collidables.get<sprite_component>(entity_static_collidable);
                        // std::cout << "Entity collidable: (" << sprite_static.grid_x << ", " << sprite_static.grid_y << ")" << " Static Entity: " << static_cast<uint32_t>(static_entity) << "\n";
                        check_collisions(
                            s_collidable, entity_proposed_x, entity_proposed_y, sprite_entity, entity_static_collidable, sprite_static_collidable,
                            transform_entity, collision_detected, x_collision, y_collision, all_x_collisions, all_y_collisions, collision_entity
                        );
                    });
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