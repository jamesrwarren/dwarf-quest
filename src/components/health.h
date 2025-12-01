#pragma once

struct hitpoints_component{ 
    int full_health_hitpoints;
    int hitpoints;
    int damage_taken_this_turn;
    bool show_damage;

    // Status effects
    bool on_fire = false;
    int on_fire_frames = 60;
    int on_fire_frames_remaining = 0;
    bool knocked_back = false;
    bool stunned = false;
    int stunned_frames = 180;
    int stunned_frames_remaining = 0;
};