#pragma once

#include <entt/entt.hpp>

struct weapon_component {
    bool visible; // Only during swing
    entt::entity owner_entt; // Who owns the weapon
};
