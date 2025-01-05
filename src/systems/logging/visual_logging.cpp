#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <entt/entt.hpp>

#include "../../config/game_config.h"
#include "../../components/path_finding.h"

struct visual_logging_system 
{
    void render(entt::registry& reg, SDL_Renderer* renderer) {
        // Set render color to red for the path nodes
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);

        // Iterate over all entities with a path_finding_component
        auto view = reg.view<path_finding_component>();
        for (entt::entity entity : view) {
            path_finding_component& path_finding = view.get<path_finding_component>(entity);

            // Iterate over each node in the path
            for (const Node& node : path_finding.path) {
                // Define the rectangle position and size based on node's grid coordinates
                SDL_Rect rect;
                rect.x = node.grid_x * GameConfig::instance().grid_cell_width;  // Assuming each grid cell is 32x32 pixels
                rect.y = node.grid_y * GameConfig::instance().grid_cell_height;
                rect.w = (GameConfig::instance().grid_cell_width / 4);  // Width of each node's visual rectangle
                rect.h = (GameConfig::instance().grid_cell_height / 4);  // Height of each node's visual rectangle

                // Render the rectangle for this node
                SDL_RenderFillRect(renderer, &rect);
            }
        }

        // Reset render color to default (optional)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    }
};