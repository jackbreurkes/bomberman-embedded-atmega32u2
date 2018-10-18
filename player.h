/**
 * Header file for implementing player functionality in bomberman game
 * Jack van Heugten Breurkes -- 23859472
 * Abhishek Kasera -- 43353168
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "system.h"
#include "setup.h"

#define PLAYER_FLASH_RATE (PACER_HZ * 0.15)


/** moves the player by diff if the corresponding position is free
    otherwise the player's position does not change
    @param Point representing the amount to move the player by
    @param pointer to the player to be moved */
void move_player_by(const Point diff, Point* player);


/** checks if the player is at a position
    @param Point representing the player's position
    @param Point to check the player's position against 
    @return whether the player is at the specified position */
bool check_for_player(const Point player, const Point check_pos);


/** draws the player at the correct position on the led matrix
    flashes the player's LED at the desired rate
    @param Point representing player's position on the LED matrix */
void draw_player(const Point draw_pos);

#endif
