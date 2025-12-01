#pragma once

#include <SDL2/SDL.h>

#include "../components/health.h"
#include "../components/item.h"

#include <entt/entt.hpp>


struct health_system 
{  
    void update(entt::registry& reg)
    {
        std::vector<entt::entity> to_destroy;
        auto view_health = reg.view<hitpoints_component>();

        // Find entities to destroy
        for (auto entity : view_health)
        {
            auto &hitpoints = view_health.get<hitpoints_component>(entity);
            if (hitpoints.hitpoints <= 0) {
                to_destroy.push_back(entity);
            }        
        }

        // Remove related weapon entities before destroying the main entity
        auto view_weapons = reg.view<weapon_component>();
        for (auto entity : to_destroy) 
        {
            for (auto weapon : view_weapons) 
            {
                auto &weapon_instance = view_weapons.get<weapon_component>(weapon);
                if (weapon_instance.owner_entt == entity) 
                {
                    reg.destroy(weapon); // Destroy the weapon first
                }
            }
            reg.destroy(entity); // Then destroy the owner
        }
    }

    void update_item_clear_up(entt::registry& reg)
    {
        auto view_items = reg.view<item_component>();
        view_items.each([&](entt::entity item_entt, item_component &item) {
            if (item.to_destroy) 
            {
                reg.destroy(item_entt); // Destroy the weapon first
            }
        });
    }
};