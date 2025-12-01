#pragma once

#include <entt/entt.hpp>

struct collision_detection_component {
    // Type of damage  (F)riendly / (E)nemy / (N)eutral
    char type = 'N';
    std::vector<entt::entity> collided_entities;
};