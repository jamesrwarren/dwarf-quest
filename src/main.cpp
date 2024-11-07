#include "game.hpp"

int main(int argc, char* argv[]) 
{
    const int target_fps = 20;               
    const int frame_delay = 1000 / target_fps;

    const int screen_width = 900;
    const int screen_height = 600;
    const char* bird_path = "bird.png";
    const int player_width = screen_width / 30;
    const int player_height = screen_height / 20;
    Uint32 now = SDL_GetTicks();
    cwt::game game;
    
    auto player_character = game.get_registry().create();
    game.get_registry().emplace<cwt::player_component>(player_character);
    game.get_registry().emplace<cwt::sprite_component>(player_character, 
        SDL_Rect{0, 0, 300, 230}, 
        SDL_Rect{10, 10, player_width, player_height}, 
        IMG_LoadTexture(game.get_renderer(), bird_path)
    );
    game.get_registry().emplace<cwt::transform_component>(player_character, 10, 10, 0, 0);
    game.get_registry().emplace<cwt::collision_detection_component>(player_character);
    

    auto enemy_character = game.get_registry().create();
    game.get_registry().emplace<cwt::sprite_component>(enemy_character, 
        SDL_Rect{0, 0, 300, 230}, 
        SDL_Rect{0, 0, player_width, player_height}, 
        IMG_LoadTexture(game.get_renderer(), bird_path)
    );
    game.get_registry().emplace<cwt::transform_component>(enemy_character, 10, 500, 0, 0);
    game.get_registry().emplace<cwt::aquire_target_component>(enemy_character);
    game.get_registry().emplace<cwt::collision_detection_component>(enemy_character);
    game.get_registry().emplace<cwt::path_finding_component>(enemy_character, now, false);


    auto enemy_character2 = game.get_registry().create();
    game.get_registry().emplace<cwt::sprite_component>(enemy_character2, 
        SDL_Rect{0, 0, 300, 230}, 
        SDL_Rect{0, 0, player_width, player_height}, 
        IMG_LoadTexture(game.get_renderer(), bird_path)
    );
    game.get_registry().emplace<cwt::transform_component>(enemy_character2, 200, 500, 0, 0);
    game.get_registry().emplace<cwt::aquire_target_component>(enemy_character2);
    game.get_registry().emplace<cwt::collision_detection_component>(enemy_character2);
    game.get_registry().emplace<cwt::path_finding_component>(enemy_character2, now, false);

    auto enemy_character3 = game.get_registry().create();
    game.get_registry().emplace<cwt::sprite_component>(enemy_character3, 
        SDL_Rect{0, 0, 300, 230}, 
        SDL_Rect{0, 0, player_width, player_height}, 
        IMG_LoadTexture(game.get_renderer(), bird_path)
    );
    game.get_registry().emplace<cwt::transform_component>(enemy_character3, 300, 500, 0, 0);
    game.get_registry().emplace<cwt::aquire_target_component>(enemy_character3);
    game.get_registry().emplace<cwt::collision_detection_component>(enemy_character3);
    game.get_registry().emplace<cwt::path_finding_component>(enemy_character3, now, false);

    auto enemy_character4 = game.get_registry().create();
    game.get_registry().emplace<cwt::sprite_component>(enemy_character4, 
        SDL_Rect{0, 0, 300, 230}, 
        SDL_Rect{0, 0, player_width, player_height}, 
        IMG_LoadTexture(game.get_renderer(), bird_path)
    );
    game.get_registry().emplace<cwt::transform_component>(enemy_character4, 400, 500, 0, 0);
    game.get_registry().emplace<cwt::aquire_target_component>(enemy_character4);
    game.get_registry().emplace<cwt::collision_detection_component>(enemy_character4);
    game.get_registry().emplace<cwt::path_finding_component>(enemy_character4, now, false);


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