/*
 * BANNER EXPLAINING FILE INFO
 * NEEDED FOR ALL SOURCE FILES
 */

#include "position.h"
#include "player.h"
#include <stdbool.h>

#define PACER_HZ 300
#define NUM_BOMBS 6
#define BOMB_FUSE (PACER_HZ * 3) // fuse time in seconds
#define SHRAPNEL_TIME (PACER_HZ * 0.3)

typedef struct bomb_s {
    uint8_t active;
    uint8_t bomb_id;
    Point pos;
    int16_t fuse;
    bool transmitted;
} Bomb;

extern Bomb bombs[NUM_BOMBS];

void bomb_at_pos(Point pos, Player* playerX, bool is_current_player);

void transmit_bomb(Point pos);

void drop_bomb(Point pos);

void enemy_bomb(Point pos);

void read_bomb(void);

void check_for_kill(Point* player_pos, Point* check_pos);

void draw_shrapnel(Point* player_pos, Point* bomb_pos, Point* bomb_draw_pos);

void draw_bombs(Point* player_pos, Point* grid_origin);
