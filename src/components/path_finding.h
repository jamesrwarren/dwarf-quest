
#pragma once

#include <vector>

#include <SDL2/SDL.h>

struct Node {
    int grid_x, grid_y;
    int g_cost = 0;
    int h_cost = 0;
    bool visited = false;
    Node* parent = nullptr;

    int f_cost() const { return g_cost + h_cost; }
};

struct path_finding_component {
    Uint32 last_path_find_time; 
    bool initialised;
    std::vector<Node> path;
    int target_node;
};