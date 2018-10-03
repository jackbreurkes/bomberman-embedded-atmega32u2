#include "system.h"
#include "avr/pio.h"
#include "pacer.h"
#include "display.h"
#include "navswitch.h"
#include "tinygl.h"

#define PACER_HZ 300

#define MAT_ROWS 7
#define MAT_COLS 5

#define MAP_ROWS 10
#define MAP_COLS 12

#define NUM_BOMBS 2
#define BOMB_FUSE (PACER_HZ * 3)
#define SHRAPNEL_TIME (PACER_HZ * 0.5)

#define PLAYER_FLASH_RATE 50

/** Define PIO pins driving LED matrix rows.  */
static const pio_t rows[] =
{
    LEDMAT_ROW1_PIO, LEDMAT_ROW2_PIO, LEDMAT_ROW3_PIO,
    LEDMAT_ROW4_PIO, LEDMAT_ROW5_PIO, LEDMAT_ROW6_PIO,
    LEDMAT_ROW7_PIO
};


/** Define PIO pins driving LED matrix columns.  */
static const pio_t cols[] =
{
    LEDMAT_COL1_PIO, LEDMAT_COL2_PIO, LEDMAT_COL3_PIO,
    LEDMAT_COL4_PIO, LEDMAT_COL5_PIO
};


/** layout of the bitmap grid is the same orientation as the board. */
static const uint8_t bitmap[MAP_ROWS][MAP_COLS] =
{
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,0,0,0,1,1,0,1},
    {1,0,1,1,0,1,1,0,1,1,0,1},
    {1,0,0,0,0,1,1,0,0,0,0,1},
    {1,0,0,0,0,1,1,0,0,0,0,1},
    {1,0,1,1,0,1,1,0,1,1,0,1},
    {1,0,1,1,0,0,0,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1}
    /*{1,1,1,1,1,1,1,0,1,1,1,1},
    {1,0,1,1,0,0,1,0,1,0,0,1},
    {1,0,0,0,0,1,0,0,1,0,1,1},
    {1,0,1,1,1,1,0,1,1,0,0,1},
    {1,0,0,0,1,0,0,1,0,0,1,1},
    {1,1,1,0,0,1,0,1,0,0,0,1},
    {1,1,1,1,0,1,0,0,0,1,1,1},
    {1,0,1,0,0,0,1,1,0,0,0,1},
    {1,0,0,0,1,0,0,0,0,1,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1}*/
};

typedef struct point_s {
    uint8_t row;
    uint8_t col;
} Point;


typedef struct player_s {
    uint8_t num;
    Point pos;
} Player;


typedef struct bomb_s {
    uint8_t active;
    char key;
    Point pos;
    int16_t fuse;
} Bomb;


static Player p1;

static Bomb bombs[NUM_BOMBS] = {
    {0, 'a', {0, 0}, 200},
    {0, 'b', {0, 0}, 200}
};

void place_bomb(uint8_t col, uint8_t row) {
    if (bitmap[row][col] == 0) {
        bombs[0].active = 1;
        bombs[0].pos.row = row;
        bombs[0].pos.col = col;
        bombs[0].fuse = BOMB_FUSE;
        //bitmap[row][col] = bombs[0].key;
    }
}


void move_player_by(Point diff) {
    p1.pos.row += diff.row;
    p1.pos.col += diff.col;

    if (bitmap[p1.pos.row][p1.pos.col] != 0) {
        p1.pos.row -= diff.row;
        p1.pos.col -= diff.col;
    }
}


void handle_player_move(void)
{
    Point move_diff = {0, 0};

    navswitch_update();

    if (navswitch_push_event_p(NAVSWITCH_NORTH)) {

        if (p1.pos.row > 0) {
            move_diff.row = -1;
        }

    } else if (navswitch_push_event_p(NAVSWITCH_SOUTH)) {

        if (p1.pos.row < MAP_ROWS - 1) {
            move_diff.row = 1;
        }

    } else if (navswitch_push_event_p(NAVSWITCH_WEST)) {

        if (p1.pos.col > 0) {
            move_diff.col = -1;
        }

    } else if (navswitch_push_event_p(NAVSWITCH_EAST)) {

        if (p1.pos.col < MAP_COLS - 1) {
            move_diff.col = 1;
        }

    }  else if (navswitch_push_event_p(NAVSWITCH_PUSH)) {

        place_bomb(p1.pos.col, p1.pos.row);

    }

    move_player_by(move_diff);
}


int main (void)
{

    system_init ();
    navswitch_init();
    pacer_init(PACER_HZ);
    //tinygl_init(TINYGL_WIDTH * 300);

    /* Initialise LED matrix pins.  */

    p1.num = 1;
    p1.pos.row = 1;
    p1.pos.col = 1;

    for (int i = 0; i < LEDMAT_COLS_NUM; i++) {
        pio_config_set(cols[i], PIO_OUTPUT_HIGH);
    }

    for (int j = 0; j < LEDMAT_ROWS_NUM; j++) {
        pio_config_set(rows[j], PIO_OUTPUT_HIGH);
    }

    int8_t draw_row_origin = 0;
    int8_t draw_col_origin = 0;

    int player_flash_counter = 0;
    int player1_flash = 1;

    while (1)
    {
        pacer_wait ();

        handle_player_move();

        int p_rel_row = 0;
        int p_rel_col = 0;

        draw_row_origin = p1.pos.row - 3;
        draw_col_origin = p1.pos.col - 2;

        if (draw_row_origin < 0) {
            p_rel_row = draw_row_origin;
            draw_row_origin = 0;
        } else if (draw_row_origin + MAT_ROWS > MAP_ROWS) {
            p_rel_row = draw_row_origin + MAT_ROWS - MAP_ROWS;
            draw_row_origin = MAP_ROWS - MAT_ROWS;
        }

        if (draw_col_origin < 0) {
            p_rel_col = draw_col_origin;
            draw_col_origin = 0;
        } else if (draw_col_origin + MAT_COLS > MAP_COLS) {
            p_rel_col = draw_col_origin + MAT_COLS - MAP_COLS;
            draw_col_origin = MAP_COLS - MAT_COLS;
        }

        //for (int row = 0; row < MAP_ROWS; row++) {
        for (int row = draw_row_origin; row < draw_row_origin + MAT_ROWS; row++) {
            //for (int col = 0; col < MAP_COLS; col++) {
            for (int col = draw_col_origin; col < draw_col_origin + MAT_COLS; col++) {
                display_pixel_set(col - draw_col_origin, row - draw_row_origin, bitmap[row][col]);
            }
        }

        for (int bomb = 0; bomb < NUM_BOMBS; bomb++) {
            if (bombs[bomb].active == 1) {
                bombs[bomb].fuse -= 1;
                if (bombs[bomb].fuse > 0) {
                    display_pixel_set(bombs[bomb].pos.col - draw_col_origin, bombs[bomb].pos.row - draw_row_origin, 1);
                } else if (bombs[bomb].fuse > -SHRAPNEL_TIME) {
                    // detonate_bomb(col, row) // breaks the map apart if 1 tile off?
                    //shouldnt go through walls??
                    // display shrapnel
                    display_pixel_set(bombs[bomb].pos.col - draw_col_origin, bombs[bomb].pos.row - draw_row_origin, 1);
                    display_pixel_set(bombs[bomb].pos.col + 1 - draw_col_origin, bombs[bomb].pos.row - draw_row_origin, 1);
                    display_pixel_set(bombs[bomb].pos.col - 1 - draw_col_origin, bombs[bomb].pos.row - draw_row_origin, 1);
                    display_pixel_set(bombs[bomb].pos.col + 2 - draw_col_origin, bombs[bomb].pos.row - draw_row_origin, 1);
                    display_pixel_set(bombs[bomb].pos.col - 2 - draw_col_origin, bombs[bomb].pos.row - draw_row_origin, 1);
                    display_pixel_set(bombs[bomb].pos.col - draw_col_origin, bombs[bomb].pos.row + 1 - draw_row_origin, 1);
                    display_pixel_set(bombs[bomb].pos.col - draw_col_origin, bombs[bomb].pos.row - 1 - draw_row_origin, 1);
                    display_pixel_set(bombs[bomb].pos.col - draw_col_origin, bombs[bomb].pos.row + 2 - draw_row_origin, 1);
                    display_pixel_set(bombs[bomb].pos.col - draw_col_origin, bombs[bomb].pos.row - 2 - draw_row_origin, 1);
                } else {
                    bombs[bomb].active = 0;
                }
            }
        }

        if (player_flash_counter < PLAYER_FLASH_RATE) {
            player_flash_counter++;
        } else {
            player1_flash = !player1_flash;
            player_flash_counter = 0;
        }
        display_pixel_set(2 + p_rel_col, 3 + p_rel_row, player1_flash);

        display_update();
    }
}
