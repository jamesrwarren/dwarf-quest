#pragma once

#include "game.hpp"
#include "../components/transform.h"
#include "../components/sprite.h"
#include "../components/collision.h"
#include "../components/collidable.h"
#include "../components/path_finding.h"
#include "../components/targetting.h"
#include "../components/sprite_animation.h"
#include "../components/health.h"
#include "../components/damage.h"
#include "../components/player.h"

#include "../config/game_config.h"

entt::entity create_player_animated(cwt::game &game, const char *texture_path, int x, int y) {
    auto player_entity = game.get_registry().create();
    int player_width = GameConfig::instance().grid_cell_width;
    int player_height = GameConfig::instance().grid_cell_height;

    game.get_registry().emplace<player_component>(player_entity);
    game.get_registry().emplace<sprite_component>(player_entity, 
        SDL_Rect{0, 0, 76, 100}, 
        SDL_Rect{x, y, player_width, player_height},
        SDL_Rect{x, y, player_width, player_height}, 
        IMG_LoadTexture(game.get_renderer(), texture_path)
    );
    game.get_registry().emplace<sprite_animation_component>(player_entity, 1, 0, 0);
    game.get_registry().emplace<transform_component>(player_entity, x, y, 0, 0);
    game.get_registry().emplace<collision_detection_component>(player_entity);
    game.get_registry().emplace<collidable_component>(player_entity);
    game.get_registry().emplace<hitpoints_component>(player_entity, 10, 10);
    game.get_registry().emplace<life_bar_component>(player_entity, player_width, 8, SDL_Color{0, 255, 0, 255}, SDL_Rect{x, y, player_width, 8});
    game.get_registry().emplace<damage_component>(player_entity, false, 1, 3000, SDL_GetTicks());

    return player_entity;
}

entt::entity create_enemy(cwt::game &game, const char *texture_path, int x, int y) {
    auto enemy_entity = game.get_registry().create();
    int enemy_width = GameConfig::instance().grid_cell_width;
    int enemy_height = GameConfig::instance().grid_cell_height;

    game.get_registry().emplace<sprite_component>(enemy_entity, 
        SDL_Rect{0, 0, 3768, 3556}, 
        SDL_Rect{x, y, enemy_width, enemy_height}, 
        SDL_Rect{x, y, enemy_width, enemy_height}, 
        IMG_LoadTexture(game.get_renderer(), texture_path)
    );
    game.get_registry().emplace<transform_component>(enemy_entity, x, y, 0, 0);
    game.get_registry().emplace<targetting_component>(enemy_entity);
    game.get_registry().emplace<collision_detection_component>(enemy_entity);
    game.get_registry().emplace<path_finding_component>(enemy_entity, SDL_GetTicks(), false);
    game.get_registry().emplace<collidable_component>(enemy_entity);
    game.get_registry().emplace<hitpoints_component>(enemy_entity, 10, 10);
    game.get_registry().emplace<life_bar_component>(enemy_entity, enemy_width, 8, SDL_Color{0, 255, 0, 255}, SDL_Rect{x, y, enemy_width, 8});
    game.get_registry().emplace<damage_component>(enemy_entity, true, 1, 3000, SDL_GetTicks());

    return enemy_entity;
}