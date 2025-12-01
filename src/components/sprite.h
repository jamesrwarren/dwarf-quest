#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

struct sprite_component{
    int src_w, src_h; // Used in sprite animation
    SDL_Rect src;
    SDL_Rect dst;
    SDL_Texture* texture;
    int grid_x, grid_y; // Grid position on the map
    bool visible;
    std::string label;
};