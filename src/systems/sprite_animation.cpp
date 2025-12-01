#pragma once

#include "../components/transform.h"
#include "../components/sprite.h"
#include "../components/sprite_animation.h"

#include <entt/entt.hpp>

struct sprite_animation_system 
{   
    void update(entt::registry& reg)
    {
        auto view = reg.view<sprite_character_animation_component, transform_component, sprite_component, hitpoints_component>();

        view.each([](sprite_character_animation_component& animation, 
                    transform_component& transform, 
                    sprite_component& sprite, 
                    hitpoints_component& hp)
        {
            // --- Direction Handling ---
            auto direction_to_index = [](Direction dir) -> int {
                switch (dir) {
                    case Direction::L:  return 0;
                    case Direction::LD: return 1;
                    case Direction::D:  return 2;
                    case Direction::RD: return 3;
                    case Direction::R:  return 4;
                    case Direction::RU: return 5;
                    case Direction::U:  return 6;
                    case Direction::LU: return 7;
                    default:            return 0;
                }
            };
            animation.sprite_direction = direction_to_index(transform.direction);

            // --- Movement Status ---
            bool is_running = (transform.vel_x != 0 || transform.vel_y != 0);
            bool is_stunned = hp.stunned;

            // --- Frame Count Update ---
            bool should_advance_frame = (!is_stunned && animation.sprite_frame_count < 2)
                                    || (is_stunned && animation.sprite_frame_count < 10);

            if (should_advance_frame) {
                animation.sprite_frame_count += 1;
            } else {
                animation.sprite_frame_count = 0;

                if (is_stunned) {
                    std::cout << "Player Stunned count: " << animation.sprite_selection_count << std::endl;
                    if (animation.sprite_selection_count > 20 && animation.sprite_selection_count < 24) {
                        animation.sprite_selection_count += 1;
                    } else {
                        animation.sprite_selection_count = 21;
                    }
                }
                else if (is_running) {
                    if (animation.sprite_selection_count < animation.sprite_x_count_last) {
                        animation.sprite_selection_count += 1;
                    } else {
                        animation.sprite_selection_count = animation.sprite_x_count_first;
                    }
                }
                else { // idle
                    if (animation.sprite_selection_count < animation.sprite_x_count_first - 1) {
                        animation.sprite_selection_count += 1;
                    } else {
                        animation.sprite_selection_count = 0;
                    }
                }
            }

            // --- Sprite Source Rect Update ---
            sprite.src.x = (animation.sprite_selection_count * sprite.src_w) + animation.padding;
            sprite.src.y = (animation.sprite_direction * sprite.src_h) + animation.padding + 10;
            sprite.src.w = sprite.src_w - (2 * animation.padding);
            sprite.src.h = sprite.src_h - (2 * animation.padding);
        });
    }

    void update_scenery_animation(entt::registry& reg)
    {
        auto view_transform = reg.view<sprite_scenery_animation_component, sprite_component>();
        view_transform.each([](sprite_scenery_animation_component &sprite_animation, sprite_component &sprite){

            if (sprite_animation.sprite_frame_count < 10) {
                sprite_animation.sprite_frame_count += 1;
            } else {
                sprite_animation.sprite_frame_count = 0;
                if (sprite_animation.sprite_selection_count < (sprite_animation.sprite_x_count - 1)) {
                    sprite_animation.sprite_selection_count += 1;
                } else {
                    sprite_animation.sprite_selection_count = 0;
                }
            }
            
            sprite.src.x = ((sprite.src_w + sprite_animation.sprite_pixel_modifier) * sprite_animation.sprite_selection_count);
            // std::cout << "SRC XY: (" << sprite.src.x << ", " << sprite.src.y << ") Select Count: " << sprite_animation.sprite_selection_count << " SRC WIDTH: " << sprite.src.w << '\n';
        });
    }
};
