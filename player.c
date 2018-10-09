#include "player.h"


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
