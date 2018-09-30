#include "avr/system.h"
#include "avr/pio.h"
#include "pacer.h"
#include "display.h"
#include "navswitch.h"

#define MAP_ROWS 7
#define MAP_COLS 5
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
    {0, 0, 0, 0, 1},
    {0, 1, 1, 0, 1},
    {0, 0, 1, 0, 1},
    {1, 0, 1, 1, 1},
    {1, 0, 1, 0, 0},
    {1, 0, 0, 1, 0},
    {1, 1, 0, 0, 0}
};



int main (void)
{
  
    system_init ();
    navswitch_init();
    pacer_init (300);
    
    /* Initialise LED matrix pins.  */
    
    for (int i = 0; i < LEDMAT_COLS_NUM; i++) {
        pio_config_set(cols[i], PIO_OUTPUT_HIGH);
    }
    
    for (int j = 0; j < LEDMAT_ROWS_NUM; j++) {
        pio_config_set(rows[j], PIO_OUTPUT_HIGH);
    }
    
    uint8_t player1_row = 0;
    uint8_t player1_col = 0;

    int cntr = 0;
    int player1_flash = 1;
    
    while (1)
    {
        pacer_wait ();
        
        /*display_column (bitmap[current_column], prev_column, current_column);
    
        prev_column = current_column;
        
        current_column++;
    
        if (current_column > (LEDMAT_COLS_NUM - 1))
        {
            current_column = 0;
        }*/
        
        navswitch_update();
        
        if (navswitch_push_event_p(NAVSWITCH_NORTH)) {
            
            if (player1_row > 0 && bitmap[player1_row - 1][player1_col] == 0) {
                player1_row--;
            }
            
        } else if (navswitch_push_event_p(NAVSWITCH_SOUTH)) {
            
            if (player1_row < MAP_ROWS - 1 && bitmap[player1_row + 1][player1_col] == 0) {
                player1_row++;
            }
            
        } else if (navswitch_push_event_p(NAVSWITCH_WEST)) {
            
            if (player1_col > 0  && bitmap[player1_row][player1_col - 1] == 0) {
                player1_col--;
            }
            
        } else if (navswitch_push_event_p(NAVSWITCH_EAST)) {
            
            if (player1_col < MAP_COLS - 1 && bitmap[player1_row][player1_col + 1] == 0) {
                player1_col++;
            }
            
        }
        

        
        
        for (int row = 0; row < MAP_ROWS; row++) {
            for (int col = 0; col < MAP_COLS; col++) {
                //if (!(row == player1_row && col == player1_col)) {
                display_pixel_set(col, row, bitmap[row][col]);
                //}
            }
        }
        
        if (cntr < PLAYER_FLASH_RATE) {
            cntr++;
        } else {
            player1_flash = !player1_flash;
            cntr = 0;
        }
        display_pixel_set(player1_col, player1_row, player1_flash);
        //use display_pixel_get(col, row) for player pos
        
        //display_pixel_set(0, 0, bitmap[0][0]);
        //display_pixel_set(1, 1, 1);
        
        display_update();
    }
}
