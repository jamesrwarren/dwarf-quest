#pragma once

#include "../components/transform.h"
#include <entt/entt.hpp>

struct transform_system 
{   
    void update(entt::registry& reg)
    {
        auto view_transform = reg.view<transform_component>();
        view_transform.each([](transform_component &transform){
            transform.pos_x += transform.vel_x;
            transform.pos_y += transform.vel_y;         
        });
    }
};