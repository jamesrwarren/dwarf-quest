#include "world/game.hpp"

#include "config/game_config.h"

#include "world/initialise_entities.cpp"

int main(int argc, char* argv[]) 
{             
    const int frame_delay = GameConfig::instance().frame_delay;

    const int player_height = GameConfig::instance().grid_cell_height;
    const int player_width = GameConfig::instance().grid_cell_width;
    cwt::game game;
    
    // Create player character
    const char* dwarf_path = "assets/images/cloud.png";
    const char* player_path = "assets/images/player_sprite_sheet_2.png";
    auto player_character = create_player_animated(game, player_path, 10, 10);
    auto player_weapon = create_weapon(game, dwarf_path, 10, 10, player_character);

    // Create enemy characters
    const char* goblin_path = "assets/images/goblin.png";
    create_enemy(game, goblin_path, 10, 500);
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