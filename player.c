/*
 * Module for implementing player functionality in bomberman game
 * Jack van Heugten Breurkes -- 23859472
 * Abhishek Kasera -- 43353168
 */


#include "player.h"
#include "display.h"
#include "bomb.h"
#include <stdbool.h>


/* moves the player by diff if the corresponding position is free
 * otherwise the player's position does not change */
void move_player_by(const Point diff, Point* player)
{
    Point new_pos = {player->row + diff.row, player->col + diff.col};

    bool in_row_bounds = new_pos.row >= 0 && new_pos.row < MAP_ROWS;
    bool in_col_bounds = new_pos.col >= 0 && new_pos.col < MAP_COLS;
    bool is_pos_free = bitmap[new_pos.row][new_pos.col] == 0;
    bool has_bomb = check_for_bomb(new_pos);

    if (in_row_bounds && in_col_bounds && is_pos_free && !has_bomb) {
        player->row = new_pos.row;
        player->col = new_pos.col;
    }
}


/* checks if the player is at the position defined by check_pos */
bool check_for_player(const Point player, const Point check_pos)
{
	bool player_found = false;
    if (player.row == check_pos.row && player.col == check_pos.col) {
        player_found = true;
    }
    return player_found;
}


/* draws the player at the correct position on the led matrix
 * flashes the player's LED at the desired rate */
void draw_player(const Point draw_pos)
{
	static uint16_t player_flash_counter = 0;
	static bool flash_state = 0;

    if (player_flash_counter < PLAYER_FLASH_RATE) {
        player_flash_counter++;
    } else {
        flash_state = !flash_state;
        player_flash_counter = 0;
    }
    display_pixel_set(draw_pos.col, draw_pos.row, flash_state);
}
