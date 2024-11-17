#include "game.hpp"

entt::entity create_player(cwt::game &game, const char *texture_path, int x, int y) {
    auto player_entity = game.get_registry().create();
    int player_width = cwt::GameConfig::instance().grid_cell_width;
    int player_height = cwt::GameConfig::instance().grid_cell_height;

    game.get_registry().emplace<cwt::player_component>(player_entity);
    game.get_registry().emplace<cwt::sprite_component>(player_entity, 
        SDL_Rect{0, 0, 1536, 1300}, 
        SDL_Rect{x, y, player_width, player_height}, 
        IMG_LoadTexture(game.get_renderer(), texture_path)
    );
    game.get_registry().emplace<cwt::transform_component>(player_entity, x, y, 0, 0);
    game.get_registry().emplace<cwt::collision_detection_component>(player_entity);

    return player_entity;
}

entt::entity create_player_animated(cwt::game &game, const char *texture_path, int x, int y) {
    auto player_entity = game.get_registry().create();
    int player_width = cwt::GameConfig::instance().grid_cell_width;
    int player_height = cwt::GameConfig::instance().grid_cell_height;

    game.get_registry().emplace<cwt::player_component>(player_entity);
    game.get_registry().emplace<cwt::sprite_component>(player_entity, 
        SDL_Rect{0, 0, 76, 100}, 
        SDL_Rect{x, y, player_width, player_height}, 
        IMG_LoadTexture(game.get_renderer(), texture_path)
    );
    game.get_registry().emplace<cwt::sprite_animation_component>(player_entity, 1, 0, 0);
    game.get_registry().emplace<cwt::transform_component>(player_entity, x, y, 0, 0);
    game.get_registry().emplace<cwt::collision_detection_component>(player_entity);
    game.get_registry().emplace<cwt::collidable_component>(player_entity);

    return player_entity;
}

entt::entity create_enemy(cwt::game &game, const char *texture_path, int x, int y) {
    auto enemy_entity = game.get_registry().create();
    int enemy_width = cwt::GameConfig::instance().grid_cell_width;
    int enemy_height = cwt::GameConfig::instance().grid_cell_height;

    game.get_registry().emplace<cwt::sprite_component>(enemy_entity, 
        SDL_Rect{0, 0, 3768, 3556}, 
        SDL_Rect{x, y, enemy_width, enemy_height}, 
        IMG_LoadTexture(game.get_renderer(), texture_path)
    );
    game.get_registry().emplace<cwt::transform_component>(enemy_entity, x, y, 0, 0);
    game.get_registry().emplace<cwt::aquire_target_component>(enemy_entity);
    game.get_registry().emplace<cwt::collision_detection_component>(enemy_entity);
    game.get_registry().emplace<cwt::path_finding_component>(enemy_entity, SDL_GetTicks(), false);
    game.get_registry().emplace<cwt::collidable_component>(enemy_entity);

    return enemy_entity;
}

int main(int argc, char* argv[]) 
{             
    const int frame_delay = cwt::GameConfig::instance().frame_delay;

    const int player_height = cwt::GameConfig::instance().grid_cell_height;
    const int player_width = cwt::GameConfig::instance().grid_cell_width;
    cwt::game game;
    
    // Create player character
    // const char* dwarf_path = "images/dwarf.png";
    const char* player_path = "images/player_sprite_sheet.png";
    auto player_character = create_player_animated(game, player_path, 10, 10);

    // Create enemy characters
    const char* goblin_path = "images/goblin.png";
    // create_enemy(game, goblin_path, 10, 500);
    // create_enemy(game, goblin_path, 200, 500);
    // create_enemy(game, goblin_path, 300, 500);
    // create_enemy(game, goblin_path, 400, 500);


    while(game.is_running()) 
    {
        Uint32 frame_start = SDL_GetTicks();

        game.read_input();
        game.update();
        game.render();

        Uint32 frame_time = SDL_GetTicks() - frame_start;

        if (frame_delay > frame_time) {
            SDL_Delay(frame_delay - frame_time);
        }
    }
    
    return 0;
}