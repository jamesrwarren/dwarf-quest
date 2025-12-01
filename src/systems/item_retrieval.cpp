#pragma once

#include <iostream>
#include "../components/item.h"
#include "../components/player.h"
#include "../components/collision.h"
#include "../components/inventory.h"
#include <entt/entt.hpp>

struct item_retrieval_system
{   
    void update(entt::registry& reg)
    {
        auto view_item_entities = reg.view<item_component>();
        auto view_player_entities = reg.view<collision_detection_component, inventory_component, player_component>();

        view_player_entities.each([&](collision_detection_component& player_collision_detection, inventory_component& player_inventory)
        {
            if (player_collision_detection.collided_entities.empty()) {
                return;
            }

            for (const entt::entity collided_entity : player_collision_detection.collided_entities) {
                // Check if collided entity is an item
                if (!view_item_entities.contains(collided_entity)) {
                    continue;
                }

                // Access item component
                item_component& item = reg.get<item_component>(collided_entity);

                // Print item name to console
                std::cout << "Player picked up item: " << item.item_name << std::endl;
                player_inventory.items.push_back(item.item_name);
                item.to_destroy = true;
            }
        });        
    }
};