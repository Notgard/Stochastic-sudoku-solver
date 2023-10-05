#ifndef __STATS_H__
#define __STATS_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/// @brief Plot sudoku solving statistics from given file
/// @param filename given file
void sudoku_plot_stats(char *filename);

/// @brief Plot multiple sudoku solving statistics from given file using the program arguments
/// @param argc number of arguments
/// @param argv arrar of arguments
void sudoku_plot_statistics(int argc, char *argv[]);

/// @brief Plot multiple sudoku solving statistics from the given file names
/// @param format all the files to plot as line graph
/// @param  
void sudoku_plot_multiple_stats(const char *format, ...);

#endif