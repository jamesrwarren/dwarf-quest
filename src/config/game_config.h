#pragma once

#include <fstream> 

struct GameConfig {
    // Public static method to access the singleton instance
    static GameConfig& instance() {
        static GameConfig instance;
        return instance;
    }
    const int grid_cell_height = 45;
    const int grid_cell_width = 45;

    const int num_rows = 20;
    const int num_columns = 30;
    // Configuration settings as public members
    std::size_t screen_height = grid_cell_height * num_rows;
    std::size_t screen_width = grid_cell_width * num_columns;

    const int target_fps = 20;               
    const int frame_delay = 1000 / target_fps;

    // Delete copy constructor and assignment operator to enforce singleton
    GameConfig(const GameConfig&) = delete;
    GameConfig& operator=(const GameConfig&) = delete;

private:
    // Private constructor to prevent multiple instances
    GameConfig() = default;
};