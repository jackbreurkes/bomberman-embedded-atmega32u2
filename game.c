/*
 * BANNER EXPLAINING FILE INFO
 * NEEDED FOR ALL SOURCE FILES
 */

#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "display.h"
#include "navswitch.h"
#include "ir_uart.h"
#include <stdbool.h>

#include "position.h"
#include "player.h"
#include "bomb.h"

#define PACER_HZ 300


#define PLAYER_FLASH_RATE 50


static Player player = {0, {0, 0}, 0};


bool check_and_handle_input(void)
{
    /* checks for input and runs functions associated with input
     * returns true if input is detected or false otherwise
     */
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
{
    system_init ();
    navswitch_init();
    pacer_init(PACER_HZ);
    display_init();
    ir_uart_init();

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
    }

    set_draw_positions(player.pos, grid_draw_origin, player_draw_pos);
    update_map(grid_draw_origin);
}


int main (void)
{
    Point player_draw_pos = {0, 0};
    Point grid_draw_origin = {0, 0}; // position of the top left LED on the LED matrix

    int player_flash_counter = 0;
    int player_flash = 1;
    bool input_registered = false;

    game_init(&grid_draw_origin, &player_draw_pos);

    char read_char = 0;
    Point pos_from_read = {0, 0};

    while (1)
    {
        pacer_wait();

        input_registered = check_and_handle_input();

        if (input_registered) {
            set_draw_positions(player.pos, &grid_draw_origin, &player_draw_pos);
            update_map(&grid_draw_origin);
        }

        read_bomb();
        draw_bombs(&player.pos, &grid_draw_origin);


        if (player_flash_counter < PLAYER_FLASH_RATE) {
            player_flash_counter++;
        } else {
            player_flash = !player_flash;
            player_flash_counter = 0;
        }


        display_pixel_set(player_draw_pos.col, player_draw_pos.row, player_flash);

        display_update();

    }
}
