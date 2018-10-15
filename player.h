#ifndef PLAYER_H
#define PLAYER_H

#include "system.h"
#include "setup.h"

#define PLAYER_FLASH_RATE 50

typedef struct player_s {
    uint8_t num;
    Point pos;
} Player;

void move_player_by(Point diff, Player* player);

void draw_player(Point* draw_pos);

#endif
