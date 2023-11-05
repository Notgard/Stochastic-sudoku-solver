#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include "config.h"

/**
 * @brief Calculates the minimum value between parameter a and b
 * 
 */
#define MIN(a,b) (((a)<(b))?(a):(b))

/// @brief Get a random double from 0 to 1 [0;1]
/// @return
double get_random(unsigned int * seed);

/// @brief Get a random integer bounded from lower bound lBound to upper bound  uBound
/// @param seed the reference of the seed used in the pseudo random number generator
/// @param lBound the lower bound of the range
/// @param uBound the upper bound of the range
/// @return a random integer bounded in range [lBound;uBound]
int get_bound_random(unsigned int *seed, unsigned int lBound, unsigned int uBound);

/// @brief Function which reads a specified file containing various sudoku puzzles, identified by their unique hash
/// and writes the puzzle's grid into a 2D integer line array
/// @param filename the specified file with the corresponding scheme:
///      12 bytes of SHA1 hash of the digits string (for randomising order)
///      81 bytes of puzzle digits
///       4 bytes of rating (nn.n)
///       3 bytes of white-space (including the linefeed);
///     100 bytes total
/// @param sudoku_dimension the dimension of the sudoku in the file
/// @param puzzle_hash the specific sudoku puzzle to read
/// @return 
int **read_sudoku_file(char *filename, size_t sudoku_dimension, char *puzzle_hash);

/// @brief Prints a sudoku grid from a 2D array, assuming the 2D grid is structured with lines and columns
/// @param sudoku_grid the provided sudoku grid
void print_sudoku(int **sudoku_grid);

/// @brief Prints a sudoku grid from a 2D array of pointers, assuming the 2D grid is structured with lines and columns
/// @param sudoku_grid the provided sudoku grid
void print_sudoku_pointers(int ***sudoku_grid);

/// @brief Randomize the given sudoku grid with random values between 1 and 9, as long as the cells aren't fixed
/// @param sudoku_grid the given sudoku grid
/// @param seed the randomization seed used
void sudoku_randomize(int ***sudoku_grid, int ** original_grid, unsigned int * seed);

/// @brief Copies the content of a sudoku grid into another
/// @param sudoku_grid the grid to modify the content of
/// @param content the content to copy
void sudoku_copy_content(int ***sudoku_grid, int **content);

/// @brief Utility function to append the results obtained from the algorithm to a file
/// @param filename the file in question, is created if doesn't exist yet. We assume it corresponds to the sudoku hash
/// @param score the value returned from the cost function in the sudoku solving algorithm
/// @param n_try the current try number
/// @param date
void sudoku_write_stats(char * filename, int score, int n_try, char * date);

/// @brief Utility function to debug the output of the sudoku solving algorithm
/// @param filename 
/// @param info 
/// @param date 
void sudoku_debug_output(char * filename, char * info, char * date);

/// @brief Prints the current configuration of the sudoku solving alogrithm
void print_config();

void print_sudoku_grid(int sudoku_grid[][SUDOKU_SIZE]);

void sudoku_pipe_write(int fd, int flag, int **grid);

void sudoku_pipe_close(int fd, int flag);

#endif