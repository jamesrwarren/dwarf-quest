#pragma once

struct sprite_animation_component{
    int sprite_direction; // down=0, right=1, left=2, up=3
    int sprite_frame_count; // frames since last change
    int sprite_selection_count; // which sprite from the sheet to choose
};