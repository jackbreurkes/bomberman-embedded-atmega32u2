/*
 * Module for implementing bomb functionality in bomberman game
 * Jack van Heugten Breurkes -- 23859472
 * Abhishek Kasera -- XXXXXXXX
 */


#include "bomb.h"
#include "system.h"
#include "ir_uart.h"
#include "display.h"
#include <stdbool.h>


/* keeps track of all the bombs available to the players.
 * the first half are assigned to the current player
 * the second half are assigned to the opponent */
Bomb bombs[NUM_BOMBS] = {
    {0, 0, {0, 0}, BOMB_FUSE},
    {0, 1, {0, 0}, BOMB_FUSE},
    {0, 2, {0, 0}, BOMB_FUSE},
    {0, 3, {0, 0}, BOMB_FUSE},
    {0, 4, {0, 0}, BOMB_FUSE},
    {0, 5, {0, 0}, BOMB_FUSE}
};


void reset_bombs(void)
/* initialises the bombs to their initial states */
{
	for (int bomb = 0; bomb < NUM_BOMBS; bomb++) {
		bombs[bomb].active = 0;
		bombs[bomb].pos.row = 0;
		bombs[bomb].pos.col = 0;
		bombs[bomb].fuse = BOMB_FUSE;
	}
}


void transmit_bomb(Point pos)
/* transmits a position to the opponent's board via a unique integer */
{
    ir_uart_putc(pos.row * MAP_COLS + pos.col);
}



static uint8_t player_bomb_num = 0;
void drop_bomb(Point pos)
/* places a player bomb at the given position */
{
    bombs[player_bomb_num].active = 1;
    bombs[player_bomb_num].pos.row = pos.row;
    bombs[player_bomb_num].pos.col = pos.col;
    bombs[player_bomb_num].fuse = BOMB_FUSE;
    if (player_bomb_num == NUM_BOMBS / 2 - 1) {
        player_bomb_num = 0;
    } else {
        player_bomb_num++;
    }
    //player->current_bomb = bomb_num;

    transmit_bomb(pos);
}



static uint8_t enemy_bomb_num = NUM_BOMBS / 2;
void enemy_bomb(Point pos)
/* places an enemy bomb at the given position */
{
    bombs[enemy_bomb_num].active = 1;
    bombs[enemy_bomb_num].pos.row = pos.row;
    bombs[enemy_bomb_num].pos.col = pos.col;
    bombs[enemy_bomb_num].fuse = BOMB_FUSE;
    if (enemy_bomb_num < NUM_BOMBS - 1) {
        enemy_bomb_num++;
    } else {
        enemy_bomb_num = NUM_BOMBS / 2;
    }
}



void read_bomb(void)
/* reads the position of an enemy bomb transmitted via infrared signal */
{
    Point pos_from_read = {0, 0};
    char read_char = 0;
    if (ir_uart_read_ready_p()) {
        read_char = ir_uart_getc();
        pos_from_read.row = (read_char) / MAP_COLS;
        pos_from_read.col = (read_char) % MAP_COLS;
        enemy_bomb(pos_from_read);
    }
}



bool check_for_bomb(Point check_pos)
/* checks if there is an active bomb at the position defined by
 * check_pos */
{
	bool point_has_bomb = false;
	for (int bomb = 0; bomb < NUM_BOMBS; bomb++) {
		if (bombs[bomb].active) {
			point_has_bomb = bombs[bomb].pos.row == check_pos.row
						     && bombs[bomb].pos.col == check_pos.col;
		}
	}
	return point_has_bomb;
}



void check_for_kill(Point* player_pos, Point* check_pos, bool* is_dead)
/* checks if the player is at the position defined by check_pos */
{
    if (player_pos->row == check_pos->row && player_pos->col == check_pos->col) {
        *is_dead = true;
    }
}



void draw_shrapnel(Point* player_pos, Point* bomb_pos, Point* bomb_draw_pos, bool* is_dead)
/* draws the shrapnel around the exploding bomb and checks if the
 * player has been killed by it */
{
    Point directions[5] = {
        {0, 0},
        {0, 1},
        {0, -1},
        {1, 0},
        {-1, 0}
    };
    Point draw_pos = {0, 0};
    Point grid_check_pos = {0, 0};
    int i = 0;
    int j = 0;

    for (int dir = 0; dir < 5; dir++) {
        i = directions[dir].row;
        j = directions[dir].col;
        draw_pos.row = bomb_draw_pos->row + i;
        draw_pos.col = bomb_draw_pos->col + j;
        grid_check_pos.row = bomb_pos->row + i;
        grid_check_pos.col = bomb_pos->col + j;

        if (bitmap[grid_check_pos.row][grid_check_pos.col] == 0) {
            display_pixel_set(draw_pos.col, draw_pos.row, 1);
            check_for_kill(player_pos, &grid_check_pos, is_dead);
            draw_pos.row += i;
            draw_pos.col += j;
            grid_check_pos.row += i;
            grid_check_pos.col += j;
            if (bitmap[grid_check_pos.row][grid_check_pos.col] == 0) {
                display_pixel_set(draw_pos.col, draw_pos.row, 1);
                check_for_kill(player_pos, &grid_check_pos, is_dead);
            }
        }
    }
}



bool draw_bombs(Point* player_pos, Point* grid_origin)
/* draws all active bombs onto the led matrix and handles countdown
 * logic. returns whether or not the player has been killed by an
 * exploding bomb */
{
    update_map(grid_origin);
    Point draw_pos = {0, 0};
    bool is_dead = false;
    for (int bomb = 0; bomb < NUM_BOMBS; bomb++) {
        if (bombs[bomb].active == 1) {
            draw_pos.row = bombs[bomb].pos.row - grid_origin->row;
            draw_pos.col = bombs[bomb].pos.col - grid_origin->col;
            bombs[bomb].fuse -= 1;
            if (bombs[bomb].fuse > 0) {
                display_pixel_set(draw_pos.col, draw_pos.row, 1);
            } else if (bombs[bomb].fuse > -SHRAPNEL_TIME) {
                draw_shrapnel(player_pos, &bombs[bomb].pos, &draw_pos, &is_dead);
            } else {
                bombs[bomb].active = 0;
            }
        }
    }
    return is_dead;
}
