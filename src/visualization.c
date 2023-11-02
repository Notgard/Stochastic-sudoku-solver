#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "config.h"
#include <curses.h>

#define ESC '\e'

void show_sudoku_grid(int sudoku[][SUDOKU_SIZE]) {
    int i, j;
    
    window_erase(stdscr);

    printw("╭-------+-------+-------╮\n");
    for (i = 0; i < SUDOKU_SIZE; i++)
    {
        for (j = 0; j < SUDOKU_SIZE; j++)
        {
            if (j == 0)
                printw("| ");
            printw("%d ", sudoku[i][j]);
            if ((j + 1) % 3 == 0)
                printw("| ");
        }
        printw("\n");
        if ((i + 1) % 3 == 0)
            printw("├-------+-------+-------┤\n");
    }
    
    window_refresh(stdscr);
}
 
int main() {
    int fd, i;

    size_t offset = 0;
    size_t bytes, bytes_read;
    int sudoku_buffer[SUDOKU_SIZE][SUDOKU_SIZE];
    int pipe_flag;

    ncurses_init();
        
    ncurses_colors();
    palette();

    if((fd = open(DATA_PIPE, O_RDONLY)) == -1) {
        perror("Error opening pipe");
        exit(EXIT_FAILURE);
    }

    printf("================================================\n");
    printf("Sudoku Solving Visualization program\n");
    printf("================================================\n");
    
    printf("Waiting for sudoku to start\n");
    while(true) {
        if((read(fd, &pipe_flag, sizeof(int))) == -1) {
            perror("Error reading named pipe flag from pipe");
            exit(EXIT_FAILURE);
        }
        if((bytes = read(fd, &sudoku_buffer[offset], sizeof(int) * SUDOKU_SIZE)) == -1) {
            perror("Error reading sudoku data from pipe");
            exit(EXIT_FAILURE);
        }

        bytes_read += bytes;

        if(bytes_read > PUZZLE_SIZE) {
            //reset offset and bytes read
            offset = 0;
            bytes_read = 0;
            show_sudoku_grid(sudoku_buffer);
        }

        offset++;
        if(pipe_flag == PIPE_CLOSE) break;
    }
    
    if(close(fd) == -1) {
        perror("Error closing pipe");
        exit(EXIT_FAILURE);
    }

    ncurses_stop();

    printf("End of program\n");
    
    return EXIT_SUCCESS;
}
