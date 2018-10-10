/*
 * BANNER EXPLAINING FILE INFO
 * NEEDED FOR ALL SOURCE FILES
 */

#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "display.h"
#include "navswitch.h"
#include "tinygl.h"
#include "led.h"
#include "ir_uart.h"
#include <stdbool.h>

#include "movement.h"
#include "player.h"
#include "bomb.h"

#define PACER_HZ 300

//#define MAP_ROWS 10
//#define MAP_COLS 12

#define MAT_MID_ROW (LEDMAT_ROWS_NUM / 2)
#define MAT_MID_COL (LEDMAT_COLS_NUM / 2)

#define NUM_BOMBS 6
#define BOMB_FUSE (PACER_HZ * 3) // fuse time in seconds
#define SHRAPNEL_TIME (PACER_HZ * 0.3)

#define PLAYER_FLASH_RATE 50


/** layout of the bitmap grid is the same orientation as the board. */
/*static const uint8_t bitmap[MAP_ROWS][MAP_COLS] =
{
    // 1s are walls, 0s are free space
    {1,1,1,0,1,1,0,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,0,0,0,1,1,0,1},
    {1,0,0,0,0,1,1,0,1,1,0,1},
    {1,0,1,1,0,1,1,0,0,0,0,0},
    {0,0,0,0,0,1,1,0,0,0,0,1},
    {1,0,1,1,0,1,1,0,1,1,0,1},
    {1,0,1,1,0,0,0,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,0,1,1,1,1,1}
};*/

/*typedef struct point_s {
    int8_t row;
    int8_t col;
} Point;*/


/*typedef struct player_s {
    uint8_t num;
    Point pos;
    uint8_t current_bomb;
} Player;*/


/*typedef struct bomb_s {
    uint8_t active;
    uint8_t bomb_id;
    Point pos;
    int16_t fuse;
} Bomb;*/


static Player player;

/*static Bomb bombs[NUM_BOMBS] = {
    {0, 0, {0, 0}, BOMB_FUSE},
    {0, 1, {0, 0}, BOMB_FUSE},
    {0, 2, {0, 0}, BOMB_FUSE},
    {0, 3, {0, 0}, BOMB_FUSE},
    {0, 4, {0, 0}, BOMB_FUSE},
    {0, 5, {0, 0}, BOMB_FUSE}
};*/


/*static uint8_t enemy_bomb_num = 0;
void bomb_at_pos(Point pos, Player* playerX, bool is_current_player) {
    if (bitmap[pos.row][pos.col] == 0) {
        if (is_current_player) {
            uint8_t bomb_num = playerX->current_bomb;
            bombs[bomb_num].active = 1;
            bombs[bomb_num].pos.row = pos.row;
            bombs[bomb_num].pos.col = pos.col;
            bombs[bomb_num].fuse = BOMB_FUSE;
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
}


void drop_bomb(Point pos, Player* playerX) {
    bomb_at_pos(pos, playerX, 1);
}

void enemy_bomb(Point pos, Player* playerX) {
    bomb_at_pos(pos, playerX, 0);
}*/


/*void move_player_by(Point diff, Player* player)
{
    Point new_pos = {player->pos.row + diff.row, player->pos.col + diff.col};

    int in_row_bounds = new_pos.row >= 0 && new_pos.row < MAP_ROWS;
    int in_col_bounds = new_pos.col >= 0 && new_pos.col < MAP_COLS;
    int is_pos_free = bitmap[new_pos.row][new_pos.col] == 0;

    if (in_row_bounds && in_col_bounds && is_pos_free) {
        player->pos.row = new_pos.row;
        player->pos.col = new_pos.col;
    }
}*/


int check_and_handle_input(void)
{
    /* checks for input and runs functions associated with input
     * returns 1 if input is detected or 0 otherwise
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
        drop_bomb(player.pos, &player);
        //bomb_at_pos(player.pos, 0);
    } else {
        input_registered = false;
    }

    if (input_registered) {
        move_player_by(move_diff, &player);
    }

    return input_registered;
}


void set_draw_positions(Point* grid_origin, Point* player_draw_pos)
{
    /* updates the map's draw origin if the map can be drawn with the
     *     player in the center
     * otherwise updates the player's position on the screen and
     *     leaves the map in place */

    Point new_grid_origin = { // map point to be drawn at top left of matrix
        player.pos.row - MAT_MID_ROW,
        player.pos.col - MAT_MID_COL
    };
    Point new_grid_close = { // map point to be drawn at the bottom right of the matrix
        new_grid_origin.row + LEDMAT_ROWS_NUM - 1,
        new_grid_origin.col + LEDMAT_COLS_NUM - 1
    };
    player_draw_pos->row = MAT_MID_ROW;
    player_draw_pos->col = MAT_MID_COL;

    if (new_grid_origin.row < 0) {
        player_draw_pos->row += new_grid_origin.row;
        grid_origin->row = 0;
    } else if (new_grid_close.row > MAP_ROWS - 1) {
        player_draw_pos->row += new_grid_close.row + 1 - MAP_ROWS;
        grid_origin->row = MAP_ROWS - LEDMAT_ROWS_NUM;
    } else {
        grid_origin->row = new_grid_origin.row;
    }

    if (new_grid_origin.col < 0) {
        player_draw_pos->col += new_grid_origin.col;
        grid_origin->col = 0;
    } else if (new_grid_close.col > MAP_COLS - 1) {
        player_draw_pos->col += new_grid_close.col + 1 - MAP_COLS;
        grid_origin->col = MAP_COLS - LEDMAT_COLS_NUM;
    } else {
        grid_origin->col = new_grid_origin.col;
    }

}


void update_map(const Point* origin)
{
    for (int row = origin->row; row < origin->row + LEDMAT_ROWS_NUM; row++) {
        for (int col = origin->col; col < origin->col + LEDMAT_COLS_NUM; col++) {
            display_pixel_set(col - origin->col, row - origin->row, bitmap[row][col]);
        }
    }
}

void game_init(Point* player_draw_pos, Point* grid_draw_origin)
{
    set_draw_positions(player_draw_pos, grid_draw_origin);
    update_map(grid_draw_origin);
}


void check_for_kill(Point* check_pos)
{
    if (player.pos.row == check_pos->row && player.pos.col == check_pos->col) {
        led_set(LED1, 1);
    }
}


void draw_shrapnel(Point* bomb_pos, Point* bomb_draw_pos)
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
            check_for_kill(&grid_check_pos);
            draw_pos.row += i;
            draw_pos.col += j;
            grid_check_pos.row += i;
            grid_check_pos.col += j;
            if (bitmap[grid_check_pos.row][grid_check_pos.col] == 0) {
                display_pixel_set(draw_pos.col, draw_pos.row, 1);
                check_for_kill(&grid_check_pos);
            }
        }
    }
}


void draw_bombs(Point* grid_origin)
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
                draw_shrapnel(&bombs[bomb].pos, &draw_pos);
            } else {
                update_map(grid_origin);
                bombs[bomb].active = 0;
            }
        }
    }
}

int main (void)
{

    system_init ();
    navswitch_init();
    pacer_init(PACER_HZ);
    display_init();
    ir_uart_init();
    //tinygl_init(TINYGL_WIDTH * 300);

    Point player_draw_pos = {0, 0};
    Point grid_draw_origin = {0, 0}; // position of the top left LED on the LED matrix


    int player_flash_counter = 0;
    int player_flash = 1;
    bool input_registered = false;

    char uart_info[3] = {0};

    bool player_chosen = false;

    int uart_step = 0;
    int uart_cycle_length = NUM_BOMBS / 2;

    while (!player_chosen) {
        navswitch_update();

        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            ir_uart_putc(1);
            player.num = 1;
            player.pos.row = 1;
            player.pos.col = 1;
            player.current_bomb = 0;
            player_chosen = true;
        }

        if (ir_uart_read_ready_p()) {
            if (ir_uart_getc() == 1) {
                player.num = 2;
                player.pos.row = MAP_ROWS - 2;
                player.pos.col = MAP_COLS - 2;
                player.current_bomb = NUM_BOMBS / 2;
                player_chosen = true;
            }
        }
    }

    set_draw_positions(&grid_draw_origin, &player_draw_pos);
    update_map(&grid_draw_origin);

    char read_char = 0;
    char prev_read_char = 0;
    while (1)
    {
        pacer_wait();

        input_registered = check_and_handle_input();

        if (input_registered) {
            set_draw_positions(&grid_draw_origin, &player_draw_pos);
            update_map(&grid_draw_origin);
        }

        draw_bombs(&grid_draw_origin);


        if (player_flash_counter < PLAYER_FLASH_RATE) {
            player_flash_counter++;
        } else {
            player_flash = !player_flash;
            player_flash_counter = 0;
        }


        display_pixel_set(player_draw_pos.col, player_draw_pos.row, player_flash);

        display_update();

        uint8_t write_bomb_id = 0;
        if (ir_uart_write_ready_p()) {
            //char c = bombs[0].pos.row * MAP_ROWS + bombs[write_bomb_id].pos.col + 1;
            if (uart_step == 0) {
                /*uart_info[0] = 'b';
                write_bomb_id = uart_step;
                if (player.num == 2) {
                    write_bomb_id += NUM_BOMBS / 2;
                }
                uart_info[1] = bombs[write_bomb_id].pos.row * MAP_ROWS + bombs[write_bomb_id].pos.col + 1;*/
                ir_uart_putc('b');
                uart_step++;
            } else if (uart_step == 1) {
                if (bombs[0].active && !bombs[0].transmitted) {
                    char c = bombs[0].pos.row * MAP_ROWS + bombs[write_bomb_id].pos.col;
                    ir_uart_putc(c);
                    bombs[0].transmitted = true;
                    uart_step++;
                }
            } else {
                //uart_info[0] = 'p';
                //uart_info[1] = player.pos.row * MAP_ROWS + player.pos.col + 1;
                uart_step = 0;
            }
            uart_info[2] = 0;
            //ir_uart_puts(uart_info);
            //ir_uart_putc(c);
        }

        Point pos_from_read = {0, 0};
        if (ir_uart_read_ready_p()) {
            read_char = ir_uart_getc();
            if (read_char == 'b') {
                pos_from_read.row = 1;//(read_char) / MAP_ROWS;
                pos_from_read.col = 1;//(read_char) % MAP_ROWS;
                enemy_bomb(pos_from_read, &player);
            } else if (prev_read_char == 'p') {
                pos_from_read.row = (read_char - 1) / MAP_ROWS;
                pos_from_read.col = (read_char - 1) % MAP_ROWS;
                // do something with player pos
            }
            //pos_from_read.row = (read_char - 1) / MAP_ROWS;
            //pos_from_read.col = (read_char - 1) % MAP_ROWS;
            //enemy_bomb(pos_from_read, &player);
            prev_read_char = read_char;
        }

    }
}
