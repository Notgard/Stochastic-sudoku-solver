#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdbool.h>

#define SUDOKU_SIZE 9
#define SUDOKU_GRID_SIZE 3

#define sudoku_cells(SIZE) (SIZE * SIZE)
#define PUZZLE_SIZE sudoku_cells(SUDOKU_SIZE)

// color constants
#define CLR_GRN "\033[0;32;32m"
#define CLR_CYN "\033[0;36m"
#define CLR_RED "\033[0;32;31m"
#define CLR_YEL "\033[1;33m"
#define CLR_BLU "\033[0;32;34m"
#define CLR_BRN "\033[0;33m"
#define CLR_RESET "\033[0m"

// configuration information of stuff in sudoku file
#define HASH_SIZE 12
#define LINE_SIZE 100

#define SUDOKU_DIR "./ressources/"
#define FILE_SIZE 256
#define DEBUG_SIZE 256

// configuration of the solving algorithm
#define MAX_TRIES 10

#define RANDOMIZE_SUDOKU (true)

#define KEEP_BEST (false)

#define KEEP_TRYING (false)

#define PRINT_CONFIG (true)

#define KEEP_START (false)

#define OLD (true)

#define TEMP_STEP 1 //the temperature is divided by two each step (depends on the numbers of tries)
#define SOLUTION_COST 0 // the cost of the wanted solution of the sudoku
#define START_TEMPERATURE ((double)(1620 / 2))
#define TEMPERATURE_CEILING 0.00273852
#define PRESUMED_PUZZLE_SIZE PUZZLE_SIZE

#define GET_STATS (false)

// gnuplot configuration
#define COMMANDE_SIZE 4086
#define LINE_THICKNESS 2
#define LINE_TYPE "lines"
#define X_RANGE 5
#define Y_RANGE 10

#define GET_OUTPUT false

extern const char plot_colors[10][10];

//visualization demo config
#define PIPE_NAME "/tmp/sudoku"
#define PIPE_OPEN 0
#define PIPE_CLOSE 1
#define WIDTH 20
#define HEIGHT 20

#endif