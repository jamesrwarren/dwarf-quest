#pragma once

#include <SDL2/SDL.h>


struct life_bar_component {
    int width;   // Width of the life bar
    int height;  // Height of the life bar (usually small)
    SDL_Color color; // Color of the life bar (typically green for full HP, red for low HP)
    SDL_Rect bar_rect; // The SDL_Rect to represent the actual life bar
};

struct damage_component{
    bool attacking; // if currently attacking
    int damage_per_hit;
    int strike_cooldown; // ms between strikes
    Uint32 last_strike; // last time char struck
 };