#pragma once

#include <vector>
#include <unordered_map>
#include <queue>

#include "../config/game_config.h"
#include "../components/sprite.h"
#include "../components/transform.h"
#include "../components/path_finding.h"
#include "../components/collidable.h"
#include "../components/targetting.h"
#include <entt/entt.hpp>

struct path_finding_system
{
    // Comparison operator for the priority queue
    struct CompareNode {
        bool operator()(const Node* a, const Node* b) const {
            return a->f_cost() > b->f_cost();
        }
    };

    // Heuristic function for A*
    static int manhattan_distance(int x1, int y1, int x2, int y2) {
        return abs(x1 - x2) + abs(y1 - y2);
    }

    inline int get_index(int x, int y) {
    return y * 100 + x;
    }

    // A* algorithm
    std::vector<Node> find_path(entt::registry& reg, int start_x, int start_y, int target_x, int target_y, std::unordered_set<int> &collidable_positions) {
        std::priority_queue<Node*, std::vector<Node*>, CompareNode> open_set;
        std::unordered_map<int, Node> all_nodes;
        static const std::array<std::pair<int, int>, 4> neighbor_offsets = {{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}};
        int target_index = get_index(target_x, target_y);

        // Initialize start node
        Node* start_node = &all_nodes[get_index(start_x, start_y)];
        start_node->grid_x = start_x;
        start_node->grid_y = start_y;
        start_node->g_cost = 0;
        start_node->h_cost =  path_finding_system::manhattan_distance(start_x, start_y, target_x, target_y);
        start_node->parent = nullptr;
        open_set.push(start_node);
        

        // A* loop
        while (!open_set.empty()) {
            Node* current = open_set.top();
            open_set.pop();

            if (current->visited) continue;
            current->visited = true;

            // Check if reached the target and return reversed path back if we have
            if (current->grid_x == target_x && current->grid_y == target_y) {
                std::vector<Node> path;
                while (current != nullptr) {
                    path.push_back(*current);
                    current = current->parent;
                }
                std::reverse(path.begin(), path.end());
                return path;
            }

            // Explore neighbors (does not do diagonals)
            for (const auto& [dx, dy] : neighbor_offsets) {
                int neighbor_x = current->grid_x + dx;
                int neighbor_y = current->grid_y + dy;

                int neighbor_index = get_index(neighbor_x, neighbor_y);

                // Check if neighbor is within bounds or already in closed set
                if (neighbor_x < 0 || neighbor_y < 0 || neighbor_x >= GameConfig::instance().num_columns || neighbor_y >= GameConfig::instance().num_rows || all_nodes[neighbor_index].visited) {
                    continue;
                }


                if (collidable_positions.count(neighbor_index) > 0) {
                    if (target_index != neighbor_index) {
                        all_nodes[neighbor_index].visited = true;
                        continue;
                    }
                }

                Node* neighbor = &all_nodes[neighbor_index];
                int tentative_g_cost = current->g_cost + 1;

                if (neighbor->g_cost == 0 || tentative_g_cost < neighbor->g_cost) {
                    neighbor->grid_x = neighbor_x;
                    neighbor->grid_y = neighbor_y;
                    neighbor->g_cost = tentative_g_cost;
                    neighbor->h_cost =  path_finding_system::manhattan_distance(neighbor_x, neighbor_y, target_x, target_y);
                    neighbor->parent = current;
                    open_set.push(neighbor);
                }
            }
        }

        // Return an empty path if no path is found
        return {};
    }

    void update(entt::registry& reg)
    {
        Uint32 now = SDL_GetTicks();
        
        std::unordered_set<int> collidable_positions;
        auto view_collidable_entities = reg.view<sprite_component, collidable_component>();
        view_collidable_entities.each([&](sprite_component &sprite) {
            collidable_positions.insert(get_index(sprite.grid_x, sprite.grid_y));
        });
        bool updated_path_this_frame = false;
        auto view_path_finding = reg.view<sprite_component, transform_component, path_finding_component, targetting_component>();
        view_path_finding.each([&](sprite_component &sprite, transform_component &transform, path_finding_component &path_finding, targetting_component &aquire_target) {           
            entt::entity target_entity = aquire_target.target_entt;

            // Check if the target entity has a sprite_component
            if (reg.valid(target_entity) && reg.all_of<sprite_component>(target_entity)) {
                // Retrieve the sprite component of the target entity
                auto &target_sprite = reg.get<sprite_component>(target_entity);

                // Grab path on initial frame for all entities
                if (!path_finding.initialised) {
                    path_finding.path = path_finding_system::find_path(reg, sprite.grid_x, sprite.grid_y, target_sprite.grid_x, target_sprite.grid_y, collidable_positions);
                    path_finding.initialised = true;
                }

                // Path finding is computationally expensive so 
                Uint32 elapsed_time = now - path_finding.last_path_find_time;
                if (elapsed_time >= (0.5 * 1000) && !updated_path_this_frame) {
                    path_finding.path = path_finding_system::find_path(reg, sprite.grid_x, sprite.grid_y, target_sprite.grid_x, target_sprite.grid_y, collidable_positions);
                    path_finding.last_path_find_time = now;
                    updated_path_this_frame = true;
                } 

                // When we get close to player just track player
                if (std::size(path_finding.path) < 3) {
                    aquire_target.target_x = aquire_target.player_x;
                    aquire_target.target_y = aquire_target.player_y;
                } else {
                    aquire_target.target_x = path_finding.path[1].grid_x * GameConfig::instance().grid_cell_width;
                    aquire_target.target_y = path_finding.path[1].grid_y * GameConfig::instance().grid_cell_height;
                }  
            }
        });
    }
};