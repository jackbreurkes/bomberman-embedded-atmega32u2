 /*
 * BANNER EXPLAINING FILE INFO
 * NEEDED FOR ALL SOURCE FILES
 */
#include "system.h"
#include "position.h"
#include "bomb.h"
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
void bomb_at_pos(Point pos, Player* playerX, bool is_current_player) {
    if (is_current_player) {
        uint8_t bomb_num = playerX->current_bomb;
        bombs[bomb_num].active = 1;
        bombs[bomb_num].pos.row = pos.row;
        bombs[bomb_num].pos.col = pos.col;
        bombs[bomb_num].fuse = BOMB_FUSE;
        bombs[bomb_num].transmitted = false;
        //if (playerX->num == 1 && bomb_num == NUM_BOMBS / 2 - 1) {
        if (bomb_num == NUM_BOMBS / 2 - 1) {
            bomb_num = 0;
        //} else if (playerX->num == 2 && bomb_num == NUM_BOMBS - 1) {
          //  bomb_num = NUM_BOMBS / 2;
        } else {
            bomb_num++;
        }
        playerX->current_bomb = bomb_num;
    } else {
        /*if (playerX->num == 1) {
            bombs[enemy_bomb_num + (NUM_BOMBS / 2)].active = 1;
            bombs[enemy_bomb_num + (NUM_BOMBS / 2)].pos.row = pos.row;
            bombs[enemy_bomb_num + (NUM_BOMBS / 2)].pos.col = pos.col;
            bombs[enemy_bomb_num + (NUM_BOMBS / 2)].fuse = BOMB_FUSE;
        } else {
            bombs[enemy_bomb_num].active = 1;
            bombs[enemy_bomb_num].pos.row = pos.row;
            bombs[enemy_bomb_num].pos.col = pos.col;
            bombs[enemy_bomb_num].fuse = BOMB_FUSE;
        }*/
        bombs[enemy_bomb_num].active = 1;
        bombs[enemy_bomb_num].pos.row = pos.row;
        bombs[enemy_bomb_num].pos.col = pos.col;
        bombs[enemy_bomb_num].fuse = BOMB_FUSE;
        if (enemy_bomb_num < NUM_BOMBS - 1) {
            enemy_bomb_num++;
        } else {
            enemy_bomb_num = NUM_BOMBS / 2;
        }
        //enemy_bomb_num = (enemy_bomb_num + 1) % (NUM_BOMBS / 2);
    }
}


void drop_bomb(Point pos, Player* player) {
    bomb_at_pos(pos, player, 1);

    char c = pos.row * MAP_COLS + pos.col;
    ir_uart_putc(c);
    //bombs[0].transmitted = true;
}

void enemy_bomb(Point pos, Player* player) {
    bomb_at_pos(pos, player, 0);
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
                update_map(grid_origin);
                bombs[bomb].active = 0;
            }
        }
    }
}
