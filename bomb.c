 /*
 * BANNER EXPLAINING FILE INFO
 * NEEDED FOR ALL SOURCE FILES
 */
#include "system.h"
#include "position.h"
#include "bomb.h"
#include "ir_uart.h"
#include "led.h"
#include "display.h"
#include <stdbool.h>

Bomb bombs[NUM_BOMBS] = {
    {0, 0, {0, 0}, BOMB_FUSE, true},
    {0, 1, {0, 0}, BOMB_FUSE, true},
    {0, 2, {0, 0}, BOMB_FUSE, true},
    {0, 3, {0, 0}, BOMB_FUSE, true},
    {0, 4, {0, 0}, BOMB_FUSE, true},
    {0, 5, {0, 0}, BOMB_FUSE, true}
};

static uint8_t enemy_bomb_num = NUM_BOMBS / 2;


void transmit_bomb(Point pos)
{
    ir_uart_putc(pos.row * MAP_COLS + pos.col);
}

void drop_bomb(Point pos, Player* player) {
    uint8_t bomb_num = player->current_bomb;
    bombs[bomb_num].active = 1;
    bombs[bomb_num].pos.row = pos.row;
    bombs[bomb_num].pos.col = pos.col;
    bombs[bomb_num].fuse = BOMB_FUSE;
    bombs[bomb_num].transmitted = false;
    if (bomb_num == NUM_BOMBS / 2 - 1) {
        bomb_num = 0;
    } else {
        bomb_num++;
    }
    player->current_bomb = bomb_num;

    transmit_bomb(pos);
}

void enemy_bomb(Point pos) {
    //bomb_at_pos(pos, player, 0);
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

void read_bomb(Player* player)
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

void check_for_kill(Point* player_pos, Point* check_pos)
{
    if (player_pos->row == check_pos->row && player_pos->col == check_pos->col) {
        led_set(LED1, 1);
    }
}

void draw_shrapnel(Point* player_pos, Point* bomb_pos, Point* bomb_draw_pos)
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
            check_for_kill(player_pos, &grid_check_pos);
            draw_pos.row += i;
            draw_pos.col += j;
            grid_check_pos.row += i;
            grid_check_pos.col += j;
            if (bitmap[grid_check_pos.row][grid_check_pos.col] == 0) {
                display_pixel_set(draw_pos.col, draw_pos.row, 1);
                check_for_kill(player_pos, &grid_check_pos);
            }
        }
    }
}


void draw_bombs(Point* player_pos, Point* grid_origin)
{
    update_map(grid_origin);
    Point draw_pos = {0, 0};
    for (int bomb = 0; bomb < NUM_BOMBS; bomb++) {
        if (bombs[bomb].active == 1) {
            draw_pos.row = bombs[bomb].pos.row - grid_origin->row;
            draw_pos.col = bombs[bomb].pos.col - grid_origin->col;
            bombs[bomb].fuse -= 1;
            if (bombs[bomb].fuse > 0) {
                display_pixel_set(draw_pos.col, draw_pos.row, 1);
            } else if (bombs[bomb].fuse > -SHRAPNEL_TIME) {
                draw_shrapnel(player_pos, &bombs[bomb].pos, &draw_pos);
            } else {
                bombs[bomb].active = 0;
            }
        }
    }
}
