#ifndef POINT_H
#define POINT_H

#include "system.h"

#define MAP_ROWS 10
#define MAP_COLS 12

typedef struct point_s {
    int8_t row;
    int8_t col;
} Point;

extern const uint8_t bitmap[MAP_ROWS][MAP_COLS];

#endif
