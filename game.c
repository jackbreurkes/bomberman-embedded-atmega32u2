/*
 * BANNER EXPLAINING FILE INFO
 * NEEDED FOR ALL SOURCE FILES
 */

#include "system.h"
#include "avr/pio.h"
#include "pacer.h"
#include "display.h"
#include "navswitch.h"
#include "tinygl.h"

#define PACER_HZ 300

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
	// 1s are walls, 0s are free space
    {1,1,1,1,1,1,0,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,0,0,0,1,1,0,1},
    {1,0,1,1,0,1,1,0,1,1,0,1},
    {1,0,0,0,0,1,1,0,0,0,0,0},
    {0,0,0,0,0,1,1,0,0,0,0,1},
    {1,0,1,1,0,1,1,0,1,1,0,1},
    {1,0,1,1,0,0,0,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,0,1,1,1,1,1}
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
    int8_t row;
    int8_t col;
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

void place_bomb(Point pos) {
    if (bitmap[pos.row][pos.col] == 0) {
        bombs[0].active = 1;
        bombs[0].pos.row = pos.row;
        bombs[0].pos.col = pos.col;
        bombs[0].fuse = BOMB_FUSE;
    }
}


void move_player_by(Point diff)
{
	Point new_pos = {p1.pos.row + diff.row, p1.pos.col + diff.col};
	
	int in_row_bounds = new_pos.row >= 0 && new_pos.row < MAP_ROWS;
	int in_col_bounds = new_pos.col >= 0 && new_pos.col < MAP_COLS;
	int is_pos_free = bitmap[new_pos.row][new_pos.col] == 0;
	
    if (in_row_bounds && in_col_bounds && is_pos_free) {
        p1.pos.row = new_pos.row;
        p1.pos.col = new_pos.col;
    }
}


void handle_input(void)
{
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

        place_bomb(p1.pos);

    }

    move_player_by(move_diff);
}


void set_draw_positions(Point* grid_origin, Point* player_pos) {
	
	Point new_grid_origin = {
		p1.pos.row - LEDMAT_ROWS_NUM / 2,
		p1.pos.col - LEDMAT_COLS_NUM / 2
	};
	
	Point player_draw_pos = {3, 2}; // where to draw the player on the matrix

	if (new_grid_origin.row < 0) {
		player_draw_pos.row += new_grid_origin.row;
	} else if (new_grid_origin.row + LEDMAT_ROWS_NUM > MAP_ROWS) {
		player_draw_pos.row += new_grid_origin.row + LEDMAT_ROWS_NUM - MAP_ROWS;
	} else {
		grid_origin->row = new_grid_origin.row;
	}

	if (new_grid_origin.col < 0) {
		player_draw_pos.col += new_grid_origin.col;
	} else if (new_grid_origin.col + LEDMAT_COLS_NUM > MAP_COLS) {
		player_draw_pos.col += new_grid_origin.col + LEDMAT_COLS_NUM - MAP_COLS;
	} else {
		grid_origin->col = new_grid_origin.col;
	}
	
	player_pos->row = player_draw_pos.row;
	player_pos->col = player_draw_pos.col;

}


int main (void)
{

    system_init ();
    navswitch_init();
    pacer_init(PACER_HZ);
    display_init();
    //tinygl_init(TINYGL_WIDTH * 300);

    p1.num = 1;
    p1.pos.row = 1;
    p1.pos.col = 1;
    
    Point player_draw_pos = {0, 0};
    Point grid_draw_origin = {0, 0}; // position of the top left LED on the LED matrix

    int player_flash_counter = 0;
    int player1_flash = 1;

    while (1)
    {
        pacer_wait();

        handle_input();
        
        set_draw_positions(&grid_draw_origin, &player_draw_pos);





        //for (int row = 0; row < MAP_ROWS; row++) {
        for (int row = grid_draw_origin.row; row < grid_draw_origin.row + LEDMAT_ROWS_NUM; row++) {
            //for (int col = 0; col < MAP_COLS; col++) {
            for (int col = grid_draw_origin.col; col < grid_draw_origin.col + LEDMAT_COLS_NUM; col++) {
                display_pixel_set(col - grid_draw_origin.col, row - grid_draw_origin.row, bitmap[row][col]);
            }
        }

        for (int bomb = 0; bomb < NUM_BOMBS; bomb++) {
            if (bombs[bomb].active == 1) {
                bombs[bomb].fuse -= 1;
                if (bombs[bomb].fuse > 0) {
                    display_pixel_set(bombs[bomb].pos.col - grid_draw_origin.col, bombs[bomb].pos.row - grid_draw_origin.row, 1);
                } else if (bombs[bomb].fuse > -SHRAPNEL_TIME) {
                    // detonate_bomb(col, row) // breaks the map apart if 1 tile off?
                    //shouldnt go through walls??
                    // display shrapnel:
                    display_pixel_set(bombs[bomb].pos.col - grid_draw_origin.col, bombs[bomb].pos.row - grid_draw_origin.row, 1);
                    display_pixel_set(bombs[bomb].pos.col + 1 - grid_draw_origin.col, bombs[bomb].pos.row - grid_draw_origin.row, 1);
                    display_pixel_set(bombs[bomb].pos.col - 1 - grid_draw_origin.col, bombs[bomb].pos.row - grid_draw_origin.row, 1);
                    display_pixel_set(bombs[bomb].pos.col + 2 - grid_draw_origin.col, bombs[bomb].pos.row - grid_draw_origin.row, 1);
                    display_pixel_set(bombs[bomb].pos.col - 2 - grid_draw_origin.col, bombs[bomb].pos.row - grid_draw_origin.row, 1);
                    display_pixel_set(bombs[bomb].pos.col - grid_draw_origin.col, bombs[bomb].pos.row + 1 - grid_draw_origin.row, 1);
                    display_pixel_set(bombs[bomb].pos.col - grid_draw_origin.col, bombs[bomb].pos.row - 1 - grid_draw_origin.row, 1);
                    display_pixel_set(bombs[bomb].pos.col - grid_draw_origin.col, bombs[bomb].pos.row + 2 - grid_draw_origin.row, 1);
                    display_pixel_set(bombs[bomb].pos.col - grid_draw_origin.col, bombs[bomb].pos.row - 2 - grid_draw_origin.row, 1);
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
                display_pixel_set(player_draw_pos.col, player_draw_pos.row, player1_flash);

        display_update();
    }
}
