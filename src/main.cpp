#include "world/game.hpp"

#include "config/game_config.h"

#include "world/initialise_entities.cpp"

int main(int argc, char* argv[]) 
{             
    const int frame_delay = GameConfig::instance().frame_delay;

    const int player_height = GameConfig::instance().grid_cell_height;
    const int player_width = GameConfig::instance().grid_cell_width;
    cwt::game game;
    
    entt::registry m_registry;

    // Create player character
    const char* weapon_path = "assets/images/sword.png";
    const char* player_path = "assets/images/player.png";
    auto player_character = create_player_animated(game, player_path, 10, 10);
    auto player_weapon = create_weapon(game, weapon_path, 10, 10, player_character);

    // Create enemy characters
    const char* zombie_path = "assets/images/zombie.png";
    
    auto zombie_1 = create_enemy(game, zombie_path, 10, 200);
    auto zombie_weapon_1 = create_weapon(game, weapon_path, 10, 10, zombie_1);

    auto zombie_2 = create_enemy(game, zombie_path, 300, 400);
    auto zombie_weapon_2 = create_weapon(game, weapon_path, 10, 10, zombie_2);

    const char* item_path = "assets/images/explosion-rays.png";
    auto item_1 = create_item(game, item_path, 350, 450, "EXPLOSION_RAY");

    const char* tree_path = "assets/images/undead_tileset/PNG/Animation1.png";
    auto tree_1 = create_scenery_animated(game, tree_path, 6, 3, 400, 500, -2);

    const char* skull_path = "assets/images/undead_tileset/PNG/Animation4.png";
    auto skull_1 = create_scenery_animated(game, skull_path, 6, 3, 950, 700, 0);

    const char* skull_path2 = "assets/images/undead_tileset/PNG/Animation5.png";
    auto skull_2 = create_scenery_animated(game, skull_path2, 6, 3, 750, 750, 0);

    const char* skull_path3 = "assets/images/undead_tileset/PNG/Animation6.png";
    auto skull_3 = create_scenery_animated(game, skull_path3, 6, 3, 860, 520, 0);

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