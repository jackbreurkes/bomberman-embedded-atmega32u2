#ifndef PLAYER_H
#define PLAYER_H

#include "system.h"
#include "position.h"

typedef struct player_s {
    uint8_t num;
    Point pos;
    uint8_t current_bomb;
} Player;

void move_player_by(Point diff, Player* player);



#endif
