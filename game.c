/*
 * Module for main Bomberman game functionality
 * Jack van Heugten Breurkes -- 23859472
 * Abhishek Kasera -- 43353168
 */

#include "system.h"
#include "pacer.h"
#include "display.h"
#include "navswitch.h"
#include "ir_uart.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"
#include <stdbool.h>

#include "setup.h"
#include "player.h"
#include "bomb.h"

#define TINYGL_TEXT_SPEED 20
#define INPUT_LOCK_TIME (PACER_HZ * 5)


static Player player = {0, {0, 0}};


/* checks for input and runs functions associated with input
 * returns true if input is detected or false otherwise */
bool check_and_handle_input(void)
{
    bool input_registered = true;
    Point move_diff = {0, 0};

    navswitch_update();

    if (navswitch_push_event_p(NAVSWITCH_NORTH)) {
        move_diff.row = -1; // move up one row
    } else if (navswitch_push_event_p(NAVSWITCH_SOUTH)) {
        move_diff.row = 1; // move down one row
    } else if (navswitch_push_event_p(NAVSWITCH_WEST)) {
        move_diff.col = -1; // move left one column
    } else if (navswitch_push_event_p(NAVSWITCH_EAST)) {
        move_diff.col = 1; // move right one column
    }  else if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
        drop_bomb(player.pos);
    } else {
        input_registered = false;
    }

    if (input_registered) {
        move_player_by(move_diff, &player);
    }

    return input_registered;
}


/* runs initialise functions for modules and ensures that players
 * begin the game on opposite sides of the map. Then initialises draw
 * positions for when the players load into the game */
void game_init(Point* grid_draw_origin, Point* player_draw_pos)
{
    system_init ();
    navswitch_init();
    pacer_init(PACER_HZ);
    tinygl_init(PACER_HZ);
    display_init();
    ir_uart_init();
    reset_bombs();

    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set(TINYGL_TEXT_SPEED);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
    tinygl_text("PRESS NAVSWITCH ");

    bool player_chosen = false;

    while (!player_chosen) {

        pacer_wait();

        navswitch_update();

        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            ir_uart_putc('p');
            player.pos.row = 1;
            player.pos.col = 1;
            player_chosen = true;
        }

        if (ir_uart_read_ready_p()) {
            if (ir_uart_getc() == 'p') {
                player.pos.row = MAP_ROWS - 2;
                player.pos.col = MAP_COLS - 2;
                player_chosen = true;
            }
        }

        tinygl_update();
    }

    set_draw_positions(player.pos, grid_draw_origin, player_draw_pos);
    update_map(grid_draw_origin);
}



/* runs the main game once
 * sets the game up, then runs the main game until the player is dead
 * then displays game over message and returns if the player touches
 * the navswitch (i.e. wants to play again). */
void play_bomberman(void)
{
    Point player_draw_pos = {0, 0};
    Point grid_draw_origin = {0, 0}; // bitmap position drawn at top left

    game_init(&grid_draw_origin, &player_draw_pos);

    bool input_registered = false;
    bool is_dead = false;

    uint16_t input_lock_timer = 0;


    while (!is_dead) {
        pacer_wait();

        input_registered = check_and_handle_input();
        if (input_registered) {
            set_draw_positions(player.pos, &grid_draw_origin, &player_draw_pos);
            update_map(&grid_draw_origin);
        }

        read_bomb();
        is_dead = draw_bombs(player.pos, grid_draw_origin);

        draw_player(player_draw_pos);

        display_update();
    }

    tinygl_text(" GAME OVER");

    input_registered = false;
    input_lock_timer = 0;
    while (!input_registered) {
        pacer_wait();
        if (input_lock_timer > INPUT_LOCK_TIME) {
            input_registered = check_and_handle_input();
        } else {
            input_lock_timer++;
        }
        tinygl_update();
    }
}


/* starts the game, plays until it is finished then restarts it */
int main(void)
{
    while (1) {
        play_bomberman();
    }
}
