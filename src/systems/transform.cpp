#pragma once

#include "../components/transform.h"
#include <entt/entt.hpp>

struct transform_system 
{   
    void update_weapons(entt::registry& reg)
    {
        // Updates position (will not pull back terrain as terrain has no transform component)
        auto view_weapon = reg.view<transform_component, weapon_component>();
        view_weapon.each([&](entt::entity entity, transform_component &transform, weapon_component &weapon){

            auto weapon_owner_transform = reg.try_get<transform_component>(weapon.owner_entt);
            
            transform.pos_x = weapon_owner_transform->pos_x;
            transform.pos_y = weapon_owner_transform->pos_y;         
        });
    }


    void update(entt::registry& reg)
    {
        auto view_transform = reg.view<transform_component>();
        view_transform.each([](transform_component &transform){
            transform.pos_x += transform.vel_x;
            transform.pos_y += transform.vel_y;         
        });
    }
};