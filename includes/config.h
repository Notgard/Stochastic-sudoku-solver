#ifndef __CONFIG_H__
#define __CONFIG_H__

#define SUDOKU_SIZE 9
#define SUDOKU_GRID_SIZE 3

#define sudoku_cells(SIZE) (SIZE * SIZE)
#define PUZZLE_SIZE sudoku_cells(SUDOKU_SIZE)

// configuration information of stuff in sudoku file
#define HASH_SIZE 12
#define LINE_SIZE 100

#define SUDOKU_DIR "./ressources/"
#define FILE_SIZE 256
#define DEBUG_SIZE 256

// configuration of the solving algorithm
#define MAX_TRIES 10
#define TEMPERATURE_CEILING 0.00273852
#define PRESUMED_PUZZLE_SIZE PUZZLE_SIZE * 2

// gnuplot configuration
#define COMMANDE_SIZE 512
#define LINE_THICKNESS 2
#define LINE_TYPE "lines"
extern const char plot_colors[6][10];

#endif