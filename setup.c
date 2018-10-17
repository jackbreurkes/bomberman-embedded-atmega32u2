/*
 * Module for handling map and positioning in Bomberman game
 * Jack van Heugten Breurkes -- 23859472
 * Abhishek Kasera -- 43353168
 */


#include "setup.h"
#include "display.h"

const uint8_t bitmap[MAP_ROWS][MAP_COLS] =
{
    /* 1s are walls, 0s are free space
    NOTE: ensure that MAP_ROWS * MAP_COLS < 256 */
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,0,0,0,1,1,0,1},
    {1,0,1,0,0,1,0,0,1,1,0,1},
    {1,0,1,1,0,1,1,0,0,0,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,1,0,1,1,0,1,1,0,1},
    {1,0,1,1,0,0,0,0,1,1,0,1},
    {1,0,1,0,0,1,1,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1}
};


void set_draw_positions(const Point player_pos, Point* grid_origin, Point* player_draw_pos)
{
    /* updates the map's draw origin if the map can be drawn with the
     *     player in the center
     * otherwise updates the player's position on the screen and
     *     leaves the map in place */

    Point new_grid_origin = { // map point to be drawn at top left of matrix
        player_pos.row - MAT_MID_ROW,
        player_pos.col - MAT_MID_COL
    };
    Point new_grid_close = { // map point to be drawn at the bottom right of the matrix
        new_grid_origin.row + LEDMAT_ROWS_NUM - 1,
        new_grid_origin.col + LEDMAT_COLS_NUM - 1
    };
    player_draw_pos->row = MAT_MID_ROW;
    player_draw_pos->col = MAT_MID_COL;

    if (new_grid_origin.row < 0) {
        player_draw_pos->row += new_grid_origin.row;
        grid_origin->row = 0;
    } else if (new_grid_close.row > MAP_ROWS - 1) {
        player_draw_pos->row += new_grid_close.row + 1 - MAP_ROWS;
        grid_origin->row = MAP_ROWS - LEDMAT_ROWS_NUM;
    } else {
        grid_origin->row = new_grid_origin.row;
    }

    if (new_grid_origin.col < 0) {
        player_draw_pos->col += new_grid_origin.col;
        grid_origin->col = 0;
    } else if (new_grid_close.col > MAP_COLS - 1) {
        player_draw_pos->col += new_grid_close.col + 1 - MAP_COLS;
        grid_origin->col = MAP_COLS - LEDMAT_COLS_NUM;
    } else {
        grid_origin->col = new_grid_origin.col;
    }
}


void update_map(const Point* origin)
/* updates the section on the map that is displayed on the LED matrix */
{
    for (int row = origin->row; row < origin->row + LEDMAT_ROWS_NUM; row++) {
        for (int col = origin->col; col < origin->col + LEDMAT_COLS_NUM; col++) {
            display_pixel_set(col - origin->col, row - origin->row, bitmap[row][col]);
        }
    }
}
