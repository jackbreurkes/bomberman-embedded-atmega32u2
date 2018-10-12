#include "player.h"
#include "display.h"


void move_player_by(Point diff, Player* player)
{
    Point new_pos = {player->pos.row + diff.row, player->pos.col + diff.col};

    int in_row_bounds = new_pos.row >= 0 && new_pos.row < MAP_ROWS;
    int in_col_bounds = new_pos.col >= 0 && new_pos.col < MAP_COLS;
    int is_pos_free = bitmap[new_pos.row][new_pos.col] == 0;

    if (in_row_bounds && in_col_bounds && is_pos_free) {
        player->pos.row = new_pos.row;
        player->pos.col = new_pos.col;
    }
}



static uint16_t player_flash_counter = 0;
static bool flash_state = 0;
void draw_player(Point* draw_pos)
{
    if (player_flash_counter < PLAYER_FLASH_RATE) {
        player_flash_counter++;
    } else {
        flash_state = !flash_state;
        player_flash_counter = 0;
    }
    display_pixel_set(draw_pos->col, draw_pos->row, flash_state);
}
