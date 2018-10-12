#include "position.h"
#include "display.h"

const uint8_t bitmap[MAP_ROWS][MAP_COLS] =
{
    // 1s are walls, 0s are free space
    // ensure that MAP_ROWS * MAP_COLUMNS < 256
    {1,1,1,0,1,1,0,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,0,0,0,1,1,0,1},
    {1,0,0,0,0,1,1,0,1,1,0,1},
    {1,0,1,1,0,1,1,0,0,0,0,0},
    {0,0,0,0,0,1,1,0,0,0,0,1},
    {1,0,1,1,0,1,1,0,1,1,0,1},
    {1,0,1,1,0,0,0,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,0,1,1,1,1,1}
};


void update_map(const Point* origin)
{
    for (int row = origin->row; row < origin->row + LEDMAT_ROWS_NUM; row++) {
        for (int col = origin->col; col < origin->col + LEDMAT_COLS_NUM; col++) {
            display_pixel_set(col - origin->col, row - origin->row, bitmap[row][col]);
        }
    }
}
