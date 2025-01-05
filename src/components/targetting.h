#pragma once

#include <entt/entt.hpp>

struct targetting_component { 
    entt::entity target_entt;
    int target_x, target_y, player_x, player_y;
 };