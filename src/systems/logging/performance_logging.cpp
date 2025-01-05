#pragma once

#include <iostream>
#include <SDL2/SDL.h>


struct performance_logging_system 
{
    Uint32 start_time = 0;   // Holds the time when start() is called
    bool is_running = false; // Tracks whether the timer is active
    Uint32 total_ms = 0;
    Uint32 total_frames = 0;

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
            total_ms += elapsed_time;
            total_frames += 1;
            std::cout << "Elapsed Time: " << elapsed_time << " ms. Average time: " << total_ms/total_frames << " ms\n";
            is_running = false;     
        }
        else 
        {
            std::cout << "Error: Timer was not started.\n";
        }
    }
};