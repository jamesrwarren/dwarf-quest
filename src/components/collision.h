#pragma once

#include <entt/entt.hpp>

struct collision_detection_component {
    std::vector<entt::entity> collided_entities;
};