#pragma once

#include "../components/transform.h"
#include "../components/sprite.h"
#include "../components/sprite_animation.h"

#include <entt/entt.hpp>

struct sprite_animation_system 
{   
    void update(entt::registry& reg)
    {
        auto view_transform = reg.view<sprite_animation_component, transform_component, sprite_component>();
        view_transform.each([](sprite_animation_component &sprite_animation, transform_component &transform, sprite_component &sprite){
            if (transform.vel_x == -1) {
                sprite_animation.sprite_direction = 2; //left
            } 
            else if (transform.vel_x == 1) {
                sprite_animation.sprite_direction = 1; //right
            } 
            else if (transform.vel_y == -1) {
                sprite_animation.sprite_direction = 3; //down
            } 
            else if (transform.vel_y == 1) {
                sprite_animation.sprite_direction = 0; //up
            } else {
                return; // standing still so exit lambda
            } 

            if (sprite_animation.sprite_frame_count < 5) {
                sprite_animation.sprite_frame_count += 1;
            } else {
                sprite_animation.sprite_frame_count = 0;
                if (sprite_animation.sprite_selection_count < 3) {
                    sprite_animation.sprite_selection_count += 1;
                } else {
                    sprite_animation.sprite_selection_count = 0;
                }
            }

            sprite.src.x = sprite.src.w * sprite_animation.sprite_selection_count;
            sprite.src.y = sprite.src.h * sprite_animation.sprite_direction; 
        });
    }
};