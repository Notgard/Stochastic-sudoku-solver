#ifndef __BENCHMARK_H__
#define __BENCHMARK_H__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

/// @brief Creates histogram of the average execution time of the sudoku solving algorithm for each sudoku difficulty using the given file
/// @param nb_tries the number of tries of the algorithm
/// @param filename the given file
void sudoku_plot_performance(int nb_tries, const char *filename);

/// @brief Creates point plot with linear regression curve of the execution time of the tests performed of the given difficulty level
/// @param argc the number of arguments
/// @param argv the list of files 
/// @param difficulty the difficulty of the tested
void sudoku_plot_difficulty_benchmark(int argc, char *argv[], const char *difficulty);

/// @brief Creates box plot of the cost distribution of each sudoku difficulty from the given file
/// @param filename the given file
void sudoku_plot_cost_per_difficulty(const char *filename);

#endif