#pragma once

#include <fstream> 

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