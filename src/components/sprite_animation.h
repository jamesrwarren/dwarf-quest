#pragma once

struct sprite_character_animation_component{
    int sprite_direction; // down=0, right=1, left=2, up=3
    int sprite_frame_count; // frames since last change
    int sprite_selection_count; // which sprite from the sheet to choose
    int sprite_x_count_first; // total sprites across first
    int sprite_x_count_last; // total sprites across last
    int padding; // padding
};

struct sprite_scenery_animation_component{
    int sprite_frame_count; // frames since last change
    int sprite_selection_count; // which sprite from the sheet to choose
    int sprite_x_count; // total sprites across
    int sprite_pixel_modifier; // how much to modify the sprites by
};