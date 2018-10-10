 /*
 * BANNER EXPLAINING FILE INFO
 * NEEDED FOR ALL SOURCE FILES
 */
#include "system.h"
#include "movement.h"
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

static uint8_t enemy_bomb_num = 0;
void bomb_at_pos(Point pos, Player* playerX, bool is_current_player) {
    if (is_current_player) {
        uint8_t bomb_num = playerX->current_bomb;
        bombs[bomb_num].active = 1;
        bombs[bomb_num].pos.row = pos.row;
        bombs[bomb_num].pos.col = pos.col;
        bombs[bomb_num].fuse = BOMB_FUSE;
        bombs[bomb_num].transmitted = false;
        if (playerX->num == 1 && bomb_num == NUM_BOMBS / 2 - 1) {
            bomb_num = 0;
        } else if (playerX->num == 2 && bomb_num == NUM_BOMBS - 1) {
            bomb_num = NUM_BOMBS / 2;
        } else {
            bomb_num++;
        }
        playerX->current_bomb = bomb_num;
    } else {
        if (playerX->num == 1) {
            bombs[enemy_bomb_num + (NUM_BOMBS / 2)].active = 1;
            bombs[enemy_bomb_num + (NUM_BOMBS / 2)].pos.row = pos.row;
            bombs[enemy_bomb_num + (NUM_BOMBS / 2)].pos.col = pos.col;
            bombs[enemy_bomb_num + (NUM_BOMBS / 2)].fuse = BOMB_FUSE;
        } else {
            bombs[enemy_bomb_num].active = 1;
            bombs[enemy_bomb_num].pos.row = pos.row;
            bombs[enemy_bomb_num].pos.col = pos.col;
            bombs[enemy_bomb_num].fuse = BOMB_FUSE;
        }
        enemy_bomb_num = (enemy_bomb_num + 1) % (NUM_BOMBS / 2);
    }
}


void drop_bomb(Point pos, Player* playerX) {
    bomb_at_pos(pos, playerX, 1);
}

void enemy_bomb(Point pos, Player* playerX) {
    bomb_at_pos(pos, playerX, 0);
}
