/*
 * BANNER EXPLAINING FILE INFO
 * NEEDED FOR ALL SOURCE FILES
 */
 
#include "point.h"

#define PACER_HZ 300
#define NUM_BOMBS 6
#define BOMB_FUSE (PACER_HZ * 3) // fuse time in seconds

typedef struct bomb_s {
    uint8_t active;
    uint8_t bomb_id;
    Point pos;
    int16_t fuse;
} Bomb;


