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
#include "../components/render_layer.h"
#include "../components/weapon.h"

#include "../config/game_config.h"

entt::entity create_player_animated(cwt::game &game, const char *texture_path, int x, int y) {
    auto player_entity = game.get_registry().create();
    int player_width = GameConfig::instance().grid_cell_width;
    int player_height = GameConfig::instance().grid_cell_height;

    int cooldown_height = 8;
    int cooldown_width = 200;
    int cooldown_x_placement = 20;
    int cooldown_y_placement = GameConfig::instance().screen_height - 10 - cooldown_height;
    int cooldown_border = 5;

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
    game.get_registry().emplace<collidable_component>(player_entity, true);
    game.get_registry().emplace<hitpoints_component>(player_entity, 10, 10);
    game.get_registry().emplace<life_bar_component>(player_entity, player_width, 8, SDL_Color{0, 255, 0, 255}, SDL_Rect{x, y, player_width, 8});
    game.get_registry().emplace<damage_component>(player_entity, false, false, 10, 0, 1, 0, 3000, SDL_GetTicks());
    game.get_registry().emplace<cooldown_component>(
        player_entity, cooldown_width, cooldown_height, 
        SDL_Color{0, 255, 0, 255}, SDL_Rect{cooldown_x_placement, cooldown_y_placement, 100, cooldown_height}, 
        SDL_Rect{(cooldown_x_placement - cooldown_border), (cooldown_y_placement - cooldown_border), (cooldown_width + (2 * cooldown_border)), (cooldown_height + (2 * cooldown_border))}
    );
    game.get_registry().emplace<layer_two_component>(player_entity);


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
    game.get_registry().emplace<collidable_component>(enemy_entity, true);
    game.get_registry().emplace<hitpoints_component>(enemy_entity, 10, 10);
    game.get_registry().emplace<life_bar_component>(enemy_entity, enemy_width, 8, SDL_Color{0, 255, 0, 255}, SDL_Rect{x, y, enemy_width, 8});
    game.get_registry().emplace<damage_component>(enemy_entity, true, false, 10, 0, 1, 0, 3000, SDL_GetTicks());
    game.get_registry().emplace<layer_two_component>(enemy_entity);

    return enemy_entity;
}

entt::entity create_weapon(cwt::game &game, const char *texture_path, int x, int y, entt::entity player_entity) {
    auto weapon_entity = game.get_registry().create();
    int weapon_width = GameConfig::instance().grid_cell_width;
    int weapon_height = GameConfig::instance().grid_cell_height;

    game.get_registry().emplace<weapon_component>(weapon_entity, false, player_entity);
    game.get_registry().emplace<sprite_component>(weapon_entity, 
        SDL_Rect{0, 0, 76, 100}, 
        SDL_Rect{x, y, weapon_width, weapon_height},
        SDL_Rect{x, y, weapon_width, weapon_height}, 
        IMG_LoadTexture(game.get_renderer(), texture_path)
    );
    game.get_registry().emplace<sprite_animation_component>(weapon_entity, 1, 0, 0);
    game.get_registry().emplace<transform_component>(weapon_entity, x, y, 0, 0);
    game.get_registry().emplace<collidable_component>(weapon_entity, false);
    game.get_registry().emplace<layer_one_component>(weapon_entity);

    return weapon_entity;
}