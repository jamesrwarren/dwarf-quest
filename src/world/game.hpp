#pragma once

#include <entt/entt.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../systems/collision.cpp"
#include "../systems/collidable.cpp"
#include "../systems/combat.cpp"
#include "../systems/damage.cpp"
#include "../systems/health.cpp"
#include "../systems/logging/text_logging.cpp"
#include "../systems/logging/visual_logging.cpp"
#include "../systems/logging/performance_logging.cpp"
#include "../systems/movement.cpp"
#include "../systems/path_finding.cpp"
#include "../systems/sprite.cpp"
#include "../systems/sprite_animation.cpp"
#include "../systems/targetting.cpp"
#include "../systems/transform.cpp"
#include "load_map.cpp"

namespace cwt {

// ----- WORLD -----
class game
{
    public: 
        game()
        {
            m_window = SDL_CreateWindow(
                "sdl window",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                GameConfig::instance().screen_width,
                GameConfig::instance().screen_height,
                SDL_WINDOW_SHOWN
            );

            if (m_window == NULL) {
                std::cout << "Could not create window: " << SDL_GetError() << '\n';
                m_is_running = false;
            }
            m_renderer = SDL_CreateRenderer(m_window, -1, 0);
            if (!m_renderer) {
                std::cout << "Error creating SDL renderer.\n";
                m_is_running = false;
            }

            m_is_running = true;

            load_map("assets/maps/map.txt", m_registry, m_renderer);
            m_collision_system.load_static_entities(m_registry);
        }
        ~game()
        {       
            SDL_DestroyWindow(m_window);
            SDL_Quit();
        }

        entt::registry& get_registry() { return m_registry; }
        SDL_Renderer* get_renderer() { return m_renderer; }

        bool is_running()
        {
            return m_is_running;
        }

        void read_input()
        {
            SDL_Event sdl_event;
            SDL_PollEvent(&sdl_event);            
            const Uint8* keystates = SDL_GetKeyboardState(NULL);

            if (keystates[SDL_SCANCODE_ESCAPE] || sdl_event.type == SDL_QUIT) {
                m_is_running = false;
            }
        }

        void update()
        {  
            m_performance_logging_system.start();
            
            // Find out where everything is heading this frame
            m_movement_system.update_players(m_registry);
            m_targetting_system.update(m_registry);
            m_path_finding_system.update(m_registry);
            m_movement_system.update_enemies(m_registry);
            m_movement_system.update_directions(m_registry);
            
            // Set weapon coords to be same as the weapon owner
            m_transform_system.update_weapons(m_registry);
            m_sprite_system.update_weapons(m_registry);
            
            // Work out where the weapons are
            m_combat_system.update_weapon_states(m_registry);
            
            // Work out collisions and damage
            m_collision_system.update(m_registry);  
            m_combat_system.update(m_registry);         
            m_damage_system.update(m_registry);

            // Finalise positions and animation frames of everything
            m_transform_system.update(m_registry);  
            m_sprite_animation_system.update(m_registry);
            m_sprite_system.update(m_registry);
            
            m_logging_system.update(m_registry, 3);          
        }

        void render()
        {
            SDL_RenderClear(m_renderer);

            m_sprite_system.render_background(m_registry, m_renderer);
            m_sprite_system.render_layer_one(m_registry, m_renderer);
            m_sprite_system.render_layer_two(m_registry, m_renderer);
            m_damage_system.render_life_bars(m_registry, m_renderer);
            m_damage_system.render_cooldowns(m_registry, m_renderer);
            m_visual_logging_system.render(m_registry, m_renderer);
            SDL_RenderPresent(m_renderer);

            m_performance_logging_system.stop();
        }

    private:
        std::size_t m_width;
        std::size_t m_height;
        SDL_Window* m_window; 
        SDL_Renderer* m_renderer;
        bool m_is_running;

        entt::registry m_registry;

        sprite_system m_sprite_system;
        sprite_animation_system m_sprite_animation_system;
        transform_system m_transform_system;
        path_finding_system m_path_finding_system;
        targetting_system m_targetting_system;
        movement_system m_movement_system;
        damage_system m_damage_system;
        combat_system m_combat_system;
        collision_system m_collision_system;
        logging_system m_logging_system;
        visual_logging_system m_visual_logging_system;
        performance_logging_system m_performance_logging_system;
};


} // namespace cwt

