#pragma once

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