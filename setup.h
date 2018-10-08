#ifndef POINT_H
#define POINT_H

#include "system.h"

#define MAP_ROWS 10
#define MAP_COLS 12

typedef struct point_s {
    int8_t row;
    int8_t col;
} Point;

typedef struct player_s {
    uint8_t num;
    Point pos;
    uint8_t current_bomb;
} Player;

extern const uint8_t bitmap[MAP_ROWS][MAP_COLS];

#endif
