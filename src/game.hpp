#pragma once

#include <any>
#include <vector>
#include <memory>
#include <cassert>
#include <iostream>
#include <queue>
#include <fstream> 
#include <algorithm>
#include <unordered_map>

#include <entt/entt.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


namespace cwt {

struct GameConfig {
    // Public static method to access the singleton instance
    static GameConfig& instance() {
        static GameConfig instance;
        return instance;
    }

    // Configuration settings as public members
    std::size_t screen_height = 800;
    std::size_t screen_width = 1200;
    
    const int num_rows = 20;
    const int num_columns = 30;
    const int grid_cell_height = screen_height / num_rows;
    const int grid_cell_width = screen_width / num_columns;
    const int target_fps = 20;               
    const int frame_delay = 1000 / target_fps;

    // Delete copy constructor and assignment operator to enforce singleton
    GameConfig(const GameConfig&) = delete;
    GameConfig& operator=(const GameConfig&) = delete;

private:
    // Private constructor to prevent multiple instances
    GameConfig() = default;
};

// Other Structs
struct Node {
    int grid_x;
    int grid_y;
    int g_cost;
    int h_cost;
    Node* parent;

    int f_cost() const { return g_cost + h_cost; }
};

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

//COMPONENTS
struct sprite_component{
    SDL_Rect src;
    SDL_Rect dst;
    SDL_Texture* texture;
    int grid_x, grid_y; // Grid position on the map
};

struct transform_component{
    int pos_x, pos_y;   // Screen position
    int vel_x, vel_y;   // Velocity
};

struct player_component{ };

struct static_component{ };

struct aquire_target_component { 
    entt::entity target_entt;
    int target_x;
    int target_y;
 };

struct collidable_component{ };

struct collision_detection_component {
    std::vector<entt::entity> collided_entities;
};

struct path_finding_component {
    Uint32 last_path_find_time; 
    bool initialised;
    std::vector<Node> path;
    int target_node;
};

// SYSTEMS
struct sprite_system 
{
    static std::pair<int, int> get_grid_position(int x, int y)
    {
        int grid_x = x / GameConfig::instance().grid_cell_width;
        int grid_y = y / GameConfig::instance().grid_cell_height;

        return std::make_pair(grid_x, grid_y);
    }

    void update(entt::registry& reg)
    {
        // Updates position (will not pull back terrain as terrain has no transform component)
        auto view_transform = reg.view<sprite_component, transform_component>();
        view_transform.each([&](entt::entity entity, sprite_component &sprite, transform_component &transform){
                sprite.dst.x = transform.pos_x;
                sprite.dst.y = transform.pos_y;
                auto [grid_x, grid_y] = get_grid_position(sprite.dst.x, sprite.dst.y);
                sprite.grid_x = grid_x;
                sprite.grid_y = grid_y;           
        });
    }

    void render(entt::registry& reg, SDL_Renderer* renderer)
    {
        // Create a view for sprite components
        auto view_sprite = reg.view<sprite_component>();

        // Use reverse iterators to render in ascending order of entity IDs
        for (auto this_sprite = view_sprite.rbegin(); this_sprite != view_sprite.rend(); ++this_sprite) {
            entt::entity entity = *this_sprite;
            
            sprite_component& sprite = view_sprite.get<sprite_component>(entity);
            // Render the sprite using SDL_RenderCopy
            SDL_RenderCopy(
                renderer, 
                sprite.texture, 
                &sprite.src, 
                &sprite.dst
            );
        }
    }
};

struct transform_system 
{   
    void update(entt::registry& reg)
    {
        auto view_transform = reg.view<transform_component>();
        view_transform.each([](transform_component &transform){
            transform.pos_x += transform.vel_x;
            transform.pos_y += transform.vel_y;            
        });
    }
};

struct collision_result {
    bool collided;
    bool collided_x;
    bool collided_y;
};

struct collidable_system 
{
    bool checkCollision(
        int src_x, int src_y, int src_width, int src_height, 
        int tgt_x, int tgt_y, int tgt_width, int tgt_height
    ) {
        return  src_x < tgt_x + tgt_width && 
                src_x + src_width > tgt_x &&
                src_y < tgt_y + tgt_height && 
                src_y + src_height > tgt_y;
    }
};

struct player_movement_system 
{  
    void update(entt::registry& reg)
    {
        const Uint8* keys = SDL_GetKeyboardState(NULL);

        // Update movement based on key inputs
        auto view_player = reg.view<transform_component, player_component>();
        view_player.each([&keys, &reg](transform_component &transform){
            // Apply movement based on input
            if (keys[SDL_SCANCODE_A]) { transform.vel_x = -1; } 
            if (keys[SDL_SCANCODE_S]) { transform.vel_y = 1; }
            if (keys[SDL_SCANCODE_W]) { transform.vel_y = -1; }
            if (keys[SDL_SCANCODE_D]) { transform.vel_x = 1; }
            if (!keys[SDL_SCANCODE_A] && !keys[SDL_SCANCODE_D]) { transform.vel_x = 0; }
            if (!keys[SDL_SCANCODE_S] && !keys[SDL_SCANCODE_W]) { transform.vel_y = 0; }    
        });
    }
};

struct enemy_movement_system 
{
    void update(entt::registry& reg)
    {
        // Get a view of all enemies (entities with aquire_target_component)
        auto view_enemies = reg.view<transform_component, aquire_target_component>();
        view_enemies.each([&](transform_component &enemy_transform, aquire_target_component &aquire_target) {

            // Calculate direction towards player
            int direction_x = aquire_target.target_x - enemy_transform.pos_x;
            int direction_y = aquire_target.target_y - enemy_transform.pos_y;

            // Set velocity to move towards the player using sign-based normalization
            enemy_transform.vel_x = (direction_x > 0) - (direction_x < 0); // -1, 0, or +1 based on the direction
            enemy_transform.vel_y = (direction_y > 0) - (direction_y < 0); // -1, 0, or +1 based on the direction
        });
    }
};

struct aquire_target_system 
{
    void update(entt::registry& reg)
    {
        // Find the player by looking for the entity with a player_component
        transform_component* player_transform = nullptr;
        entt::entity player_entity = entt::null; // Initialize as null
        
        auto view_player = reg.view<transform_component, player_component>();
        view_player.each([&player_transform, &player_entity](entt::entity entity_player, transform_component &transform) {
            player_transform = &transform; 
            player_entity = entity_player;
        });

        // If the player is found
        if (player_transform) 
        {
            // Get a view of all enemies (entities with aquire_target_component)
            auto view_enemies = reg.view<transform_component, aquire_target_component>();

            // Update each enemy's velocity to move towards the player
            view_enemies.each([&](transform_component &enemy_transform, aquire_target_component &aquire_target) {
                aquire_target.target_entt = player_entity;
                aquire_target.target_x = player_transform->pos_x;
                aquire_target.target_y = player_transform->pos_y;
            });
        }
    }
};

struct path_finding_system
{
    // Comparison operator for the priority queue
    struct CompareNode {
        bool operator()(const Node* a, const Node* b) const {
            return a->f_cost() > b->f_cost();
        }
    };

    // Heuristic function for A*
    static int manhattan_distance(int x1, int y1, int x2, int y2) {
        return abs(x1 - x2) + abs(y1 - y2);
    }

    // A* algorithm
    std::vector<Node> find_path(entt::registry& reg, int start_x, int start_y, int target_x, int target_y) {
        std::priority_queue<Node*, std::vector<Node*>, CompareNode> open_set;
        std::unordered_map<int, Node> all_nodes;
        std::unordered_map<int, bool> closed_set;

        auto get_index = [](int x, int y) { return y * 100 + x; };  // Unique index for grid position

        // Initialize start node
        Node* start_node = &all_nodes[get_index(start_x, start_y)];
        start_node->grid_x = start_x;
        start_node->grid_y = start_y;
        start_node->g_cost = 0;
        start_node->h_cost =  path_finding_system::manhattan_distance(start_x, start_y, target_x, target_y);
        start_node->parent = nullptr;
        open_set.push(start_node);

        // A* loop
        while (!open_set.empty()) {
            Node* current = open_set.top();
            open_set.pop();

            // Check if reached the target and return reversed path back if we have
            if (current->grid_x == target_x && current->grid_y == target_y) {
                std::vector<Node> path;
                while (current != nullptr) {
                    path.push_back(*current);
                    current = current->parent;
                }
                std::reverse(path.begin(), path.end());
                return path;
            }

            closed_set[get_index(current->grid_x, current->grid_y)] = true;

            // Explore neighbors (does not do diagonals)
            for (const auto& [dx, dy] : std::vector<std::pair<int, int>>{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}) {
                int neighbor_x = current->grid_x + dx;
                int neighbor_y = current->grid_y + dy;

                int neighbor_index = get_index(neighbor_x, neighbor_y);

                // Check if neighbor is within bounds or already in closed set
                if (neighbor_x < 0 || neighbor_y < 0 || neighbor_x >= GameConfig::instance().num_columns || neighbor_y >= GameConfig::instance().num_rows || closed_set[neighbor_index]) {
                    continue;
                }

                // Check for collision and add collidables to closed set immediately
                auto view_collidable_entities = reg.view<sprite_component, collidable_component>();
                bool collision_detected = false;
                // TODO - index this?
                view_collidable_entities.each([&](sprite_component &sprite) {
                    if (sprite.grid_x == neighbor_x && sprite.grid_y == neighbor_y) {
                        collision_detected = true;
                        return;
                    }
                });

                if (collision_detected) {
                    closed_set[neighbor_index] = true;  // Mark collidable as visited
                    continue;
                }

                Node* neighbor = &all_nodes[neighbor_index];
                int tentative_g_cost = current->g_cost + 1;

                if (neighbor->g_cost == 0 || tentative_g_cost < neighbor->g_cost) {
                    neighbor->grid_x = neighbor_x;
                    neighbor->grid_y = neighbor_y;
                    neighbor->g_cost = tentative_g_cost;
                    neighbor->h_cost =  path_finding_system::manhattan_distance(neighbor_x, neighbor_y, target_x, target_y);
                    neighbor->parent = current;
                    open_set.push(neighbor);
                }
            }
        }

        // Return an empty path if no path is found
        return {};
    }

    void update(entt::registry& reg)
    {
        Uint32 now = SDL_GetTicks();
        bool updated_path_this_frame = false;
        auto view_path_finding = reg.view<sprite_component, transform_component, path_finding_component, aquire_target_component>();
        view_path_finding.each([&](sprite_component &sprite, transform_component &transform, path_finding_component &path_finding, aquire_target_component &aquire_target) {           
            entt::entity target_entity = aquire_target.target_entt;

            // Check if the target entity has a sprite_component
            if (reg.valid(target_entity) && reg.all_of<sprite_component>(target_entity)) {
                // Retrieve the sprite component of the target entity
                auto &target_sprite = reg.get<sprite_component>(target_entity);

                // Grab path on initial frame for all entities
                if (!path_finding.initialised) {
                    path_finding.path = path_finding_system::find_path(reg, sprite.grid_x, sprite.grid_y, target_sprite.grid_x, target_sprite.grid_y);
                    path_finding.initialised = true;
                }

                // Path finding is computationally expensive so 
                Uint32 elapsed_time = now - path_finding.last_path_find_time;
                if (elapsed_time >= (0.5 * 1000) && !updated_path_this_frame) {
                    path_finding.path = path_finding_system::find_path(reg, sprite.grid_x, sprite.grid_y, target_sprite.grid_x, target_sprite.grid_y);
                    path_finding.last_path_find_time = now;
                    updated_path_this_frame = true;
                } 
                aquire_target.target_x = path_finding.path[1].grid_x * GameConfig::instance().grid_cell_width;
                aquire_target.target_y = path_finding.path[1].grid_y * GameConfig::instance().grid_cell_height;    
            }
        });
    }
};

struct collision_system 
{
    // Static grid map - populated once with static objects
    std::unordered_map<std::pair<int, int>, std::vector<entt::entity>, pair_hash> static_grid_map;

    // Function to initialize static grid map
    void load_static_entities(entt::registry& reg) {
        auto view_static = reg.view<sprite_component, collidable_component, static_component>();
        view_static.each([&](entt::entity entity, sprite_component &sprite) {
            std::pair<int, int> cell = { sprite.grid_x, sprite.grid_y };
            static_grid_map[cell].push_back(entity);
        });
    }

    void update(entt::registry& reg)
    {
        collidable_system s_collidable;
        auto view_collision = reg.view<sprite_component, collidable_component, transform_component>();

        // Map to store entities by grid cell
        std::unordered_map<std::pair<int, int>, std::vector<entt::entity>, pair_hash> grid_map;

        // Fill the grid map with entities
        view_collision.each([&](entt::entity entity, sprite_component &sprite, transform_component &transform) {
            std::pair<int, int> cell = { sprite.grid_x, sprite.grid_y };
            grid_map[cell].push_back(entity);
        });

        // Now, perform collision detection with nearby entities
        auto view_entity = reg.view<transform_component, sprite_component, collision_detection_component>();
        view_entity.each([&](entt::entity entity, transform_component &transform_entity, sprite_component &sprite_entity, collision_detection_component &collision_entity) {
            // Create vector for this 
            std::vector<entt::entity> cell_entities;
            cell_entities.clear();
            int entity_proposed_x = transform_entity.pos_x + transform_entity.vel_x;
            int entity_proposed_y = transform_entity.pos_y + transform_entity.vel_y;

            // Only check the current cell and its neighbors
            bool collision_detected = false;
            bool x_collision = true;
            bool y_collision = true;

            for (int dx = -2; dx <= 2; ++dx) {
                for (int dy = -2; dy <= 2; ++dy) {
                    std::pair<int, int> neighbor_cell = { sprite_entity.grid_x + dx, sprite_entity.grid_y + dy };

                    if (static_grid_map.find(neighbor_cell) != static_grid_map.end()) {
                        cell_entities = static_grid_map[neighbor_cell];
                    }

                    if (grid_map.find(neighbor_cell) != grid_map.end()) {
                        cell_entities.insert(
                            cell_entities.end(),                  // Insert at the end of cell_entities
                            grid_map[neighbor_cell].begin(),       // Start of grid_map vector
                            grid_map[neighbor_cell].end()          // End of grid_map vector
                        );
                    }

                    // Check if the neighbor cell has any entities
                    // if (grid_map.find(neighbor_cell) != grid_map.end()) {
                    for (entt::entity entity_collidable : cell_entities) {
                        // Skip self-collision check
                        if (entity == entity_collidable) continue;

                        // Access the collidable entity's sprite_component
                        sprite_component& sprite_collidable = view_collision.get<sprite_component>(entity_collidable);

                        // Check for collision
                        if (s_collidable.checkCollision(
                            entity_proposed_x, entity_proposed_y, sprite_entity.dst.w, sprite_entity.dst.h, 
                            sprite_collidable.dst.x, sprite_collidable.dst.y, sprite_collidable.dst.w, sprite_collidable.dst.h))
                        {
                            // Check separately for x and y collisions if moving diagonally
                            if (transform_entity.vel_x != 0 && transform_entity.vel_y != 0) {
                                x_collision = s_collidable.checkCollision(
                                    entity_proposed_x, transform_entity.pos_y, sprite_entity.dst.w, sprite_entity.dst.h, 
                                    sprite_collidable.dst.x, sprite_collidable.dst.y, sprite_collidable.dst.w, sprite_collidable.dst.h
                                );
                                y_collision = s_collidable.checkCollision(
                                    transform_entity.pos_x, entity_proposed_y, sprite_entity.dst.w, sprite_entity.dst.h, 
                                    sprite_collidable.dst.x, sprite_collidable.dst.y, sprite_collidable.dst.w, sprite_collidable.dst.h
                                );
                            }
                            collision_detected = true;
                            break;  // Early exit if collision detected
                        }
                    }
                    if (collision_detected) break;
                }
                if (collision_detected) break;
            }

            // If a collision was detected, revert the entity's velocity
            if (collision_detected) {
                if (x_collision) {
                    transform_entity.vel_x = 0;
                }
                if (y_collision) {
                    transform_entity.vel_y = 0;
                }
            }
        });
    }

    void update_basic(entt::registry& reg)
    {
        collidable_system s_collidable;
        auto view_collision = reg.view<sprite_component, collidable_component>();

        auto view_entity = reg.view<transform_component, sprite_component, collision_detection_component>();
        view_entity.each([&s_collidable, &view_collision](entt::entity entity, transform_component &transform_entity, sprite_component &sprite_entity, collision_detection_component &collision_entity) {
            int entity_proposed_x = transform_entity.pos_x + transform_entity.vel_x;
            int entity_proposed_y = transform_entity.pos_y + transform_entity.vel_y;

            // Collision detection
            bool collision_detected = false;
            bool x_collision = true;
            bool y_collision = true;
            view_collision.each([&](entt::entity entity_collidable, sprite_component &sprite_collidable) {
                if (entity == entity_collidable) return; // Skip self-collision check

                // Check for collision
                if (s_collidable.checkCollision(
                    entity_proposed_x, entity_proposed_y, sprite_entity.dst.w, sprite_entity.dst.h, 
                    sprite_collidable.dst.x, sprite_collidable.dst.y, sprite_collidable.dst.w, sprite_collidable.dst.h))
                {
                    if (transform_entity.vel_x != 0 && transform_entity.vel_y != 0) {

                        x_collision = s_collidable.checkCollision(
                            entity_proposed_x, transform_entity.pos_y, sprite_entity.dst.w, sprite_entity.dst.h, 
                            sprite_collidable.dst.x, sprite_collidable.dst.y, sprite_collidable.dst.w, sprite_collidable.dst.h
                        );
                        y_collision = s_collidable.checkCollision(
                            transform_entity.pos_x, entity_proposed_y, sprite_entity.dst.w, sprite_entity.dst.h, 
                            sprite_collidable.dst.x, sprite_collidable.dst.y, sprite_collidable.dst.w, sprite_collidable.dst.h
                        );
                    }
                    collision_detected = true;
                    return;  // Early exit if collision detected
                }
            });


            // If a collision was detected, revert the entity's velocity
            if (collision_detected) {
                if (x_collision) {
                    transform_entity.vel_x = 0;
                }
                if (y_collision) {
                    transform_entity.vel_y = 0;
                }         
            };
        });
    }
};

struct logging_system 
{
    Uint32 last_print_time = SDL_GetTicks();
    
    void update(entt::registry& reg, int log_interval)
    {
        Uint32 now = SDL_GetTicks();
        Uint32 elapsed_time = now - last_print_time;
        
        if (elapsed_time >= (log_interval * 1000)) {
            // Players State
            auto view_players = reg.view<sprite_component, transform_component, player_component>();
            view_players.each([&](entt::entity player_entity, sprite_component &player_sprite, transform_component &player_transform) {
                std::cout << "Player ID: " << static_cast<uint32_t>(player_entity) 
                << " -- X,Y: (" << player_transform.pos_x << "," << player_transform.pos_y
                << ") -- Grid X,Y: (" << player_sprite.grid_x << "," << player_sprite.grid_y << ")" << '\n';
            });
        
            // Enemies State
            auto view_enemies = reg.view<sprite_component, transform_component, path_finding_component, aquire_target_component>();
            view_enemies.each([&](entt::entity enemy_entity, sprite_component &enemy_sprite, transform_component &enemy_transform, path_finding_component &enemy_path_finding, aquire_target_component &enemy_targetting) {
                std::cout << "Enemy ID: " << static_cast<uint32_t>(enemy_entity) << '\n'; 
                std::cout << "Path from (" << enemy_sprite.grid_x << ", " << enemy_sprite.grid_y << ")" << "\n";
                for (const auto& node : enemy_path_finding.path) {
                    std::cout << "Node at (" << node.grid_x << ", " << node.grid_y << ") with f-cost: " << node.f_cost() << '\n';
                }
            });

            // std::cout << entity_proposed_x << "," << transform_entity.pos_y << '\n';
            // std::cout << transform_entity.pos_x << "," << entity_proposed_y << '\n';
            // std::cout << sprite_collidable.dst.x << "," << sprite_collidable.dst.x + sprite_collidable.dst.w << "," << sprite_collidable.dst.y << "," << sprite_collidable.dst.y + sprite_collidable.dst.h << '\n';

            last_print_time = now;
        
        }
    }
};

struct performance_logging_system 
{
    Uint32 start_time = 0;   // Holds the time when start() is called
    bool is_running = false; // Tracks whether the timer is active

    // Starts the timer
    void start() 
    {
        start_time = SDL_GetTicks();
        is_running = true;
    }

    // Stops the timer and prints the elapsed time
    void stop() 
    {
        if (is_running) 
        {
            Uint32 now = SDL_GetTicks();
            Uint32 elapsed_time = now - start_time;
            std::cout << "Elapsed Time: " << elapsed_time << " ms\n";
            is_running = false;
        }
        else 
        {
            std::cout << "Error: Timer was not started.\n";
        }
    }
};

struct visual_logging_system 
{
void render(entt::registry& reg, SDL_Renderer* renderer) {
        // Set render color to red for the path nodes
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);

        // Iterate over all entities with a path_finding_component
        auto view = reg.view<path_finding_component>();
        for (entt::entity entity : view) {
            path_finding_component& path_finding = view.get<path_finding_component>(entity);

            // Iterate over each node in the path
            for (const Node& node : path_finding.path) {
                // Define the rectangle position and size based on node's grid coordinates
                SDL_Rect rect;
                rect.x = node.grid_x * GameConfig::instance().grid_cell_width;  // Assuming each grid cell is 32x32 pixels
                rect.y = node.grid_y * GameConfig::instance().grid_cell_height;
                rect.w = (GameConfig::instance().grid_cell_width / 4);  // Width of each node's visual rectangle
                rect.h = (GameConfig::instance().grid_cell_height / 4);  // Height of each node's visual rectangle

                // Render the rectangle for this node
                SDL_RenderFillRect(renderer, &rect);
            }
        }

        // Reset render color to default (optional)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    }
};

void load_map(const std::string& filename, entt::registry& registry, SDL_Renderer* renderer)
{   
    std::ifstream map_file(filename);
    if (!map_file.is_open()) {
        std::cerr << "Error: Could not open map file " << filename << '\n';
        return;
    }
    const int tile_width = GameConfig::instance().grid_cell_width;
    const int tile_height = GameConfig::instance().grid_cell_height;
    int row = 0;

    sprite_system sprite_system_inst;

    std::string line;
    while (std::getline(map_file, line)) {
        for (int col = 0; col < line.size(); ++col) {
            char tile = line[col];
            if (tile == 'd' || tile == 'g') {
                // Create entity for this tile
                entt::entity entity = registry.create();

                // Assign sprite component
                sprite_component& sprite = registry.emplace<sprite_component>(entity);
                registry.emplace<static_component>(entity);

                // The image being used for the sprite
                sprite.src.x = 0;
                sprite.src.y = 0;
                sprite.src.w = 200;
                sprite.src.h = 200;

                sprite.dst.x = col * tile_width;
                sprite.dst.y = row * tile_height;
                sprite.dst.w = tile_width;
                sprite.dst.h = tile_height;

                auto [grid_x, grid_y] = sprite_system_inst.get_grid_position(sprite.dst.x, sprite.dst.y);
                sprite.grid_x = grid_x;
                sprite.grid_y = grid_y;

                // Load texture based on tile type
                if (tile == 'd') {
                    sprite.texture = IMG_LoadTexture(renderer, "images/dirt.jpeg");
                    registry.emplace<collidable_component>(entity);
                    std::cout << "Loaded DIRT" << std::endl;
                } else if (tile == 'g') {
                    sprite.texture = IMG_LoadTexture(renderer, "images/grass.png");
                    std::cout << "Loaded GRASS" << std::endl;
                }

                if (!sprite.texture) {
                    std::cerr << "Error loading texture for tile: " << SDL_GetError() << '\n';
                }
            }
        }
        ++row;
    }
}

// WORLD
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
                NULL
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

            load_map("config/map.txt", m_registry, m_renderer);
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
            m_player_movement_system.update(m_registry);
            m_aquire_target_system.update(m_registry);
            m_path_finding_system.update(m_registry);
            
            m_enemy_movement_system.update(m_registry);
            m_collision_system.update(m_registry);           
            m_transform_system.update(m_registry);
            m_sprite_system.update(m_registry);
            
            m_logging_system.update(m_registry, 3);          
        }

        void render()
        {
            SDL_RenderClear(m_renderer);

            m_sprite_system.render(m_registry, m_renderer);
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
        transform_system m_transform_system;
        path_finding_system m_path_finding_system;
        aquire_target_system m_aquire_target_system;
        enemy_movement_system m_enemy_movement_system;
        player_movement_system m_player_movement_system;
        collision_system m_collision_system;
        logging_system m_logging_system;
        visual_logging_system m_visual_logging_system;
        performance_logging_system m_performance_logging_system;
};


} // namespace cwt

