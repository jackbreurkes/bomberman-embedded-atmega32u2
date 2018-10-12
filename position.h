#ifndef POSITION_H
#define POSITION_H

#include "system.h"

#define MAP_ROWS 10
#define MAP_COLS 12

#define MAT_MID_ROW (LEDMAT_ROWS_NUM / 2)
#define MAT_MID_COL (LEDMAT_COLS_NUM / 2)

typedef struct point_s {
    int8_t row;
    int8_t col;
} Point;

extern const uint8_t bitmap[MAP_ROWS][MAP_COLS];

void set_draw_positions(const Point player_pos, Point* grid_origin, Point* player_draw_pos);

void update_map(const Point* origin);

#endif
