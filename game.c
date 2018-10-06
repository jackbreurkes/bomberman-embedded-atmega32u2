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
#include "led.h"

#define PACER_HZ 300

#define MAP_ROWS 10
#define MAP_COLS 12

#define MAT_MID_ROW (LEDMAT_ROWS_NUM / 2)
#define MAT_MID_COL (LEDMAT_COLS_NUM / 2)

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


int check_and_handle_input(void)
{
	/* checks for input and runs functions associated with input
	 * returns 1 if input is detected or 0 otherwise 
	 */
	int input_registered = 1;
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
    } else {
		input_registered = 0;
	}

	if (input_registered == 1) {
		move_player_by(move_diff);
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
		p1.pos.row - MAT_MID_ROW,
		p1.pos.col - MAT_MID_COL
	};
	Point new_grid_close = { // map point to be drawn at the bottom right of the matrix
		new_grid_origin.row + LEDMAT_ROWS_NUM - 1,
		new_grid_origin.col + LEDMAT_COLS_NUM - 1
	};
	player_draw_pos->row = MAT_MID_ROW;
	player_draw_pos->col = MAT_MID_COL;

	if (new_grid_origin.row < 0) {
		player_draw_pos->row += new_grid_origin.row;
	} else if (new_grid_close.row > MAP_ROWS - 1) {
		player_draw_pos->row += new_grid_close.row + 1 - MAP_ROWS;
	} else {
		grid_origin->row = new_grid_origin.row;
	}

	if (new_grid_origin.col < 0) {
		player_draw_pos->col += new_grid_origin.col;
	} else if (new_grid_close.col > MAP_COLS - 1) {
		player_draw_pos->col += new_grid_close.col + 1 - MAP_COLS;
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
	if (p1.pos.row == check_pos->row && p1.pos.col == check_pos->col) {
		led_set(LED1, 1);
	}
}


void draw_shrapnel(Point* bomb_pos, Point* bomb_draw_pos) {
	
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
    //tinygl_init(TINYGL_WIDTH * 300);

    p1.num = 1;
    p1.pos.row = 1;
    p1.pos.col = 1;
    
    Point player_draw_pos = {p1.pos.row, p1.pos.col};
    Point grid_draw_origin = {0, 0}; // position of the top left LED on the LED matrix

	//check_and_handle_input();
	//set_draw_positions(&player_draw_pos, &grid_draw_origin);
	update_map(&grid_draw_origin);

    int player_flash_counter = 0;
    int player1_flash = 1;
    int input_registered = 0;

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
            player1_flash = !player1_flash;
            player_flash_counter = 0;
        }
        
        display_pixel_set(player_draw_pos.col, player_draw_pos.row, player1_flash);

        display_update();
    }
}
