/*
 * BANNER EXPLAINING FILE INFO
 * NEEDED FOR ALL SOURCE FILES
 */
 
#include "movement.h"
#include "player.h"
#include <stdbool.h>

#define PACER_HZ 300
#define NUM_BOMBS 6
#define BOMB_FUSE (PACER_HZ * 3) // fuse time in seconds

typedef struct bomb_s {
    uint8_t active;
    uint8_t bomb_id;
    Point pos;
    int16_t fuse;
} Bomb;

extern Bomb bombs[NUM_BOMBS];

void bomb_at_pos(Point pos, Player* playerX, bool is_current_player);

void drop_bomb(Point pos, Player* playerX);

void enemy_bomb(Point pos, Player* playerX);
