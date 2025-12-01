#pragma once

#include <SDL2/SDL.h>

struct damage_component{
    int damage_per_hit;
    bool apply_damage;

    // Status effects applied by damage
    bool fire = false;
    bool knock_back = false;
    bool stun = false;

    // Type of damage  (F)riendly / (E)nemy / (N)eutral
    char type = 'N';
 };