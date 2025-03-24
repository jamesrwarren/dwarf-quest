#pragma once

#include "../components/transform.h"
#include "../components/sprite.h"
#include "../components/sprite_animation.h"

#include <entt/entt.hpp>

struct sprite_animation_system 
{   
    void update(entt::registry& reg)
    {
        int padding = 30;
        
        auto view_transform = reg.view<sprite_animation_component, transform_component, sprite_component>();
        view_transform.each([&padding](sprite_animation_component &sprite_animation, transform_component &transform, sprite_component &sprite){
            bool running = true;
            bool hit = false;

            if (transform.vel_x == -1 && transform.vel_y == -1) {
                sprite_animation.sprite_direction = 1; //up-left
            } else if (transform.vel_x == 1 && transform.vel_y == -1) {
                sprite_animation.sprite_direction = 3; //up-right
            } else if (transform.vel_x == 1 && transform.vel_y == 1) {
                sprite_animation.sprite_direction = 5; //right-down
            } else if (transform.vel_x == -1 && transform.vel_y == 1) {
                sprite_animation.sprite_direction = 7; //down-left
            } else if (transform.vel_x == -1) {
                sprite_animation.sprite_direction = 0; //left
            } else if (transform.vel_x == 1) {
                sprite_animation.sprite_direction = 4; //right
            } else if (transform.vel_y == -1) {
                sprite_animation.sprite_direction = 2; //up
            } else if (transform.vel_y == 1) {
                sprite_animation.sprite_direction = 6; //down
            } 

            if (transform.vel_y == 0 && transform.vel_x == 0) {
                running = false;
            }

            if (sprite_animation.sprite_frame_count < 2) {
                sprite_animation.sprite_frame_count += 1;
            } else {
                if (running) {
                    sprite_animation.sprite_frame_count = 0;
                    if (sprite_animation.sprite_selection_count < 11) {
                        sprite_animation.sprite_selection_count += 1;
                    } else {
                        sprite_animation.sprite_selection_count = 4;
                    }
                } else {
                    sprite_animation.sprite_frame_count = 0;
                    if (sprite_animation.sprite_selection_count < 3) {
                        sprite_animation.sprite_selection_count += 1;
                    } else {
                        sprite_animation.sprite_selection_count = 0;
                    }
                }
            }

            sprite.src.x = (sprite.src_w * sprite_animation.sprite_selection_count) + padding;
            sprite.src.y = (sprite.src_h * sprite_animation.sprite_direction) + padding + 10;
            sprite.src.h = sprite.src_h - (2 * padding);
            sprite.src.w = sprite.src_w - (2 * padding);
        });
    }
};

// L UL U UR R RD D DL

// void update(entt::registry& reg)
//     {
//         auto view_transform = reg.view<sprite_animation_component, transform_component, sprite_component>();
//         view_transform.each([](sprite_animation_component &sprite_animation, transform_component &transform, sprite_component &sprite){
//             if (transform.vel_x == -1) {
//                 sprite_animation.sprite_direction = 2; //left
//             } 
//             else if (transform.vel_x == 1) {
//                 sprite_animation.sprite_direction = 1; //right
//             } 
//             else if (transform.vel_y == -1) {
//                 sprite_animation.sprite_direction = 3; //down
//             } 
//             else if (transform.vel_y == 1) {
//                 sprite_animation.sprite_direction = 0; //up
//             } else {
//                 return; // standing still so exit lambda
//             } 

//             if (sprite_animation.sprite_frame_count < 5) {
//                 sprite_animation.sprite_frame_count += 1;
//             } else {
//                 sprite_animation.sprite_frame_count = 0;
//                 if (sprite_animation.sprite_selection_count < 3) {
//                     sprite_animation.sprite_selection_count += 1;
//                 } else {
//                     sprite_animation.sprite_selection_count = 0;
//                 }
//             }

//             sprite.src.x = sprite.src.w * sprite_animation.sprite_selection_count;
//             sprite.src.y = sprite.src.h * sprite_animation.sprite_direction; 
//         });
//     }