/**
 * Header file for implementing bomb functionality in bomberman game
 * Jack van Heugten Breurkes -- 23859472
 * Abhishek Kasera -- 43353168
 */

#include "setup.h"
#include "player.h"
#include <stdbool.h>

#define NUM_BOMBS 6
#define BOMB_FUSE (PACER_HZ * 3) // PACER_HZ * fuse time in seconds
#define SHRAPNEL_TIME (PACER_HZ * 0.3)


/** defines the Bomb struct */
typedef struct bomb_s {
    bool active;
    uint8_t bomb_id;
    Point pos;
    int16_t fuse;
} Bomb;


/** resets the bombs to their initial states */
void reset_bombs(void);


/** transmits a position to the opponent's board via a unique integer
    @param a Point struct representing the position to transmit */
void transmit_bomb(const Point pos);


/** places a player bomb at the given position
    @param a Point struct representing the position to drop a bomb at */
void drop_bomb(const Point pos);


/** places an enemy bomb at the given position
    @param a Point struct representing the position to drop a bomb at */
void enemy_bomb(const Point pos);


/** reads the position of an enemy bomb transmitted via infrared signal */
void read_bomb(void);


/** checks if there is an active bomb at the given position
    @param the position to check as a Point struct
    @return whether there is a bomb at the given position */
bool check_for_bomb(const Point check_pos);


/** draws the shrapnel around the exploding bomb and checks if the
    player has been killed by it
    @param Point representing the player's position
    @param Point at which the bomb has exploded
    @param Point representing the bomb's position on the LED matrix
    @return whether the player has been killed */
bool draw_shrapnel(const Point player, const Point bomb_pos, const Point bomb_draw_pos);


/** draws all active bombs onto the led matrix and handles countdown logic.
    @param Point representing the player's position
    @param Point representing the map position at the top left of the LED matrix
    @return bool that is true if player has been killed by an exploding bomb */
bool draw_bombs(const Point player, const Point grid_origin);
