/*
 * Module for main Bomberman game functionality
 * Jack van Heugten Breurkes -- 23859472
 * Abhishek Kasera -- 
 */

#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "display.h"
#include "navswitch.h"
#include "ir_uart.h"
#include <stdbool.h>

#include "setup.h"
#include "player.h"
#include "bomb.h"


static Player player = {0, {0, 0}};


bool check_and_handle_input(void)
/* checks for input and runs functions associated with input
 * returns true if input is detected or false otherwise */
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


void game_init(Point* grid_draw_origin, Point* player_draw_pos)
/* runs initialise functions for modules and ensures that one player
 * is player 1 and the other is player 2 */
{
    system_init ();
    navswitch_init();
    pacer_init(PACER_HZ);
    display_init();
    ir_uart_init();
    
    display_pixel_set(LEDMAT_COLS_NUM / 2, LEDMAT_ROWS_NUM / 2, 1);

    bool player_chosen = false;

    while (!player_chosen) {
        navswitch_update();

        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            ir_uart_putc('p');
            player.num = 1;
            player.pos.row = 1;
            player.pos.col = 1;
            player_chosen = true;
        }

        if (ir_uart_read_ready_p()) {
            if (ir_uart_getc() == 'p') {
                player.num = 2;
                player.pos.row = MAP_ROWS - 2;
                player.pos.col = MAP_COLS - 2;
                player_chosen = true;
            }
        }
        
        display_update();
    }

    set_draw_positions(player.pos, grid_draw_origin, player_draw_pos);
    update_map(grid_draw_origin);
}


int main (void)
{
    Point player_draw_pos = {0, 0};
    Point grid_draw_origin = {0, 0}; // bitmap position drawn at top left

    game_init(&grid_draw_origin, &player_draw_pos);

    bool input_registered = false;
    //bool is_dead = false;

    while (1)
    {
        pacer_wait();

        input_registered = check_and_handle_input();
        if (input_registered) {
            set_draw_positions(player.pos, &grid_draw_origin, &player_draw_pos);
            update_map(&grid_draw_origin);
        }

        read_bomb();
        //is_dead = draw_bombs(&player.pos, &grid_draw_origin);
        draw_bombs(&player.pos, &grid_draw_origin);
        
        draw_player(&player_draw_pos);

        display_update();
    }
}
