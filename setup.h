#ifndef POSITION_H
#define POSITION_H

#include "system.h"

#define MAP_ROWS 10
#define MAP_COLS 12

#define MAT_MID_ROW (LEDMAT_ROWS_NUM / 2)
#define MAT_MID_COL (LEDMAT_COLS_NUM / 2)

#define PACER_HZ 300


/** represents map points using a row and column parameter */
typedef struct point_s {
    int8_t row;
    int8_t col;
} Point;


extern const uint8_t bitmap[MAP_ROWS][MAP_COLS];


/** updates the map's draw origin if it can be drawn with the player centered
    otherwise updates the player's position on the screen
    @param Point representing the player's position
    @param pointer to the Point representing the top left map position
    @param pointer to the Point representing the player's position on the LED matrix */
void set_draw_positions(const Point player, Point* grid_origin, Point* player_draw_pos);


/** updates the section of the map that is displayed on the LED matrix
    @param pointer to the Point representing the top left map position */
void update_map(const Point* origin);

#endif
