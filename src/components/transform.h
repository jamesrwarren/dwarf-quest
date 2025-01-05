#pragma once

#include <string>

enum class Direction {
    U, // Up
    D, // Down
    L, // Left
    R, //Right
    RD, // Right-Down
    RU, // Right-Up
    LD, // Left-Down
    LU, // Left-Up
    // Add more directions as needed
};

struct transform_component{
    int pos_x, pos_y;   // Screen position
    int vel_x, vel_y;   // Velocity
    Direction direction;
};