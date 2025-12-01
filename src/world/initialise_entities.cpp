#pragma once

#include <utility>

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
#include "../components/combat.h"
#include "../components/player.h"
#include "../components/render_layer.h"
#include "../components/weapon.h"
#include "../components/item.h"
#include "../components/inventory.h"

#include "../config/game_config.h"

std::pair<int, int> get_source_dimensions(
    cwt::game &game, 
    const char *texture_path, 
    int num_sprites_x, 
    int num_sprites_y
) 
{
    SDL_Texture* texture = IMG_LoadTexture(game.get_renderer(), texture_path);
    if (!texture) {
        SDL_Log("Failed to load texture: %s", SDL_GetError());
        // Handle error (e.g., return {0, 0}, throw exception, etc.)
        return {0, 0};
    }

    int texture_width = 0, texture_height = 0;
    SDL_QueryTexture(texture, nullptr, nullptr, &texture_width, &texture_height);

    // std::cout << "TOTAL WIDTH/HEIGHT: " << texture_width << ", " << texture_height << '\n';

    int src_w = texture_width / num_sprites_x;
    int src_h = texture_height / num_sprites_y;
    return {src_w, src_h}; // return as a pair
}

entt::entity create_player_animated(cwt::game &game, const char *texture_path, int x, int y) {
    auto player_entity = game.get_registry().create();
    int player_width = GameConfig::instance().grid_cell_width;
    int player_height = GameConfig::instance().grid_cell_height;
    int cooldown_height = 8;
    int cooldown_width = 200;
    int cooldown_x_placement = 20;
    int cooldown_y_placement = GameConfig::instance().screen_height - 10 - cooldown_height;
    int cooldown_border = 5;

    int num_sprites_x = 32;
    int num_sprites_y = 8;
    int first_sprite_index = 4;
    int last_sprite_index = 11;
    int padding = 30;
    auto [src_w, src_h] = get_source_dimensions(game, texture_path, num_sprites_x, num_sprites_y);

    game.get_registry().emplace<player_component>(player_entity);
    game.get_registry().emplace<sprite_component>(player_entity, 
        128, 128,
        SDL_Rect{0, 0, src_w, src_h}, 
        SDL_Rect{x, y, player_width, player_height},
        IMG_LoadTexture(game.get_renderer(), texture_path),
        0, 0,
        true,
        std::string("PLAYER")
    );
    game.get_registry().emplace<sprite_character_animation_component>(player_entity, 1, 0, 0, first_sprite_index, last_sprite_index, padding);
    game.get_registry().emplace<transform_component>(player_entity, x, y, 0, 0, 4);
    game.get_registry().emplace<collision_detection_component>(player_entity, 'F');
    game.get_registry().emplace<collidable_component>(player_entity, true);
    game.get_registry().emplace<hitpoints_component>(player_entity, 10, 10);
    game.get_registry().emplace<life_bar_component>(player_entity, player_width, 8, SDL_Color{0, 255, 0, 255}, SDL_Rect{x, y, player_width, 8});
    game.get_registry().emplace<combat_component>(player_entity, false, false, 10, 0, 0, 3000, SDL_GetTicks());
    game.get_registry().emplace<cooldown_component>(
        player_entity, cooldown_width, cooldown_height, 
        SDL_Color{0, 255, 0, 255}, SDL_Rect{cooldown_x_placement, cooldown_y_placement, 100, cooldown_height}, 
        SDL_Rect{(cooldown_x_placement - cooldown_border), (cooldown_y_placement - cooldown_border), (cooldown_width + (2 * cooldown_border)), (cooldown_height + (2 * cooldown_border))}
    );
    game.get_registry().emplace<inventory_component>(player_entity);
    game.get_registry().emplace<layer_two_component>(player_entity);


    return player_entity;
}

entt::entity create_enemy(cwt::game &game, const char *texture_path, int x, int y) {
    auto enemy_entity = game.get_registry().create();
    int enemy_width = GameConfig::instance().grid_cell_width;
    int enemy_height = GameConfig::instance().grid_cell_height;
    
    int num_sprites_x = 32;
    int num_sprites_y = 8;
    int first_sprite_index = 4;
    int last_sprite_index = 11;
    int padding = 30;
    auto [src_w, src_h] = get_source_dimensions(game, texture_path, num_sprites_x, num_sprites_y);

    game.get_registry().emplace<sprite_component>(enemy_entity, 
        128, 128,
        SDL_Rect{0, 0, src_w, src_h}, 
        SDL_Rect{x, y, enemy_width, enemy_height}, 
        IMG_LoadTexture(game.get_renderer(), texture_path),
        0, 0,
        true,
        std::string("ENEMY")
    );
    game.get_registry().emplace<sprite_character_animation_component>(enemy_entity, 1, 0, 0, first_sprite_index, last_sprite_index, padding);
    game.get_registry().emplace<transform_component>(enemy_entity, x, y, 0, 0, 2);
    game.get_registry().emplace<targetting_component>(enemy_entity);
    game.get_registry().emplace<collision_detection_component>(enemy_entity, 'E');
    game.get_registry().emplace<collidable_component>(enemy_entity, true);
    game.get_registry().emplace<path_finding_component>(enemy_entity, SDL_GetTicks(), false);
    game.get_registry().emplace<hitpoints_component>(enemy_entity, 10, 10);
    game.get_registry().emplace<life_bar_component>(enemy_entity, enemy_width, 8, SDL_Color{0, 255, 0, 255}, SDL_Rect{x, y, enemy_width, 8});
    game.get_registry().emplace<combat_component>(enemy_entity, true, false, 10, 0, 0, 3000, SDL_GetTicks());
    game.get_registry().emplace<layer_two_component>(enemy_entity);

    return enemy_entity;
}

entt::entity create_weapon(cwt::game &game, const char *texture_path, int x, int y, entt::entity char_entity) {
    auto weapon_entity = game.get_registry().create();
    int weapon_width = GameConfig::instance().grid_cell_width;
    int weapon_height = GameConfig::instance().grid_cell_height;
    auto collision_detection_char = game.get_registry().try_get<collision_detection_component>(char_entity);

    game.get_registry().emplace<weapon_component>(weapon_entity, char_entity);
    game.get_registry().emplace<damage_component>(weapon_entity, 1, false, false, false, true, collision_detection_char->type);
    game.get_registry().emplace<sprite_component>(weapon_entity,
        325, 743,
        SDL_Rect{0, 0, 325, 743}, 
        SDL_Rect{x, y, weapon_width, weapon_height},
        IMG_LoadTexture(game.get_renderer(), texture_path),
        0, 0,
        false,
        std::string("WEAPON")
    );
    game.get_registry().emplace<transform_component>(weapon_entity, x, y, 0, 0);
    game.get_registry().emplace<collidable_component>(weapon_entity, false);
    game.get_registry().emplace<layer_one_component>(weapon_entity);

    return weapon_entity;
}

entt::entity create_item(cwt::game &game, const char *texture_path, int x, int y, std::string item_name) {
    auto item_entity = game.get_registry().create();
    int item_width = GameConfig::instance().grid_cell_width;
    int item_height = GameConfig::instance().grid_cell_height;

    game.get_registry().emplace<sprite_component>(item_entity, 
        512, 512,
        SDL_Rect{0, 0, 512, 512}, 
        SDL_Rect{x, y, item_width, item_height}, 
        IMG_LoadTexture(game.get_renderer(), texture_path),
        0, 0,
        true,
        std::string("ITEM")
    );
    game.get_registry().emplace<transform_component>(item_entity, x, y, 0, 0, 0);
    game.get_registry().emplace<collidable_component>(item_entity, true);
    game.get_registry().emplace<item_component>(item_entity, item_name);
    game.get_registry().emplace<layer_two_component>(item_entity);

    return item_entity;
}

entt::entity create_scenery_animated(
    cwt::game &game, 
    const char *texture_path, 
    int num_sprites_x, 
    int num_sprites_y, 
    int x, int y,
    int pixel_offset
) 
{
    auto scenery_entity = game.get_registry().create();
    int scenery_width = GameConfig::instance().grid_cell_width;
    int scenery_height = GameConfig::instance().grid_cell_height;

    auto [src_w, src_h] = get_source_dimensions(game, texture_path, num_sprites_x, num_sprites_y);

    game.get_registry().emplace<sprite_component>(scenery_entity, 
        src_w, src_h,
        SDL_Rect{0, 0, src_w, src_h}, 
        SDL_Rect{x, y, scenery_width * 2, scenery_height * 2},
        IMG_LoadTexture(game.get_renderer(), texture_path),
        0, 0,
        true,
        std::string("SCENERY")
    );
    game.get_registry().emplace<sprite_scenery_animation_component>(scenery_entity, 0, 0, num_sprites_x, pixel_offset);
    game.get_registry().emplace<transform_component>(scenery_entity, x, y, 0, 0, 0);
    game.get_registry().emplace<collidable_component>(scenery_entity, true);
    game.get_registry().emplace<layer_two_component>(scenery_entity);

    return scenery_entity;
}
