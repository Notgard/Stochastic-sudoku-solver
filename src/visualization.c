#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#include <ncurses.h>

#include "config.h"
#include "colors.h"
#include "functions.h"
#include "window.h"

#define ESC '\e'

void print_uline(window_t *window)
{
    char *line = "-------------------------------------------------------------------------";
    int line_size = SUDOKU_GRID_SIZE + (SUDOKU_GRID_SIZE - 1) + 2;
    window_addch(window, ACS_ULCORNER);
    for (int k = 0; k < SUDOKU_GRID_SIZE; k++)
    {
        if (k == SUDOKU_GRID_SIZE - 1)
            window_printw(window, "%.*s", line_size, line);
        else
            window_printw(window, "%.*s+", line_size, line);
    }
    window_addch(window, ACS_URCORNER);
    window_printw(window, "\n");
}

void print_bline(window_t *window)
{
    char *line = "-------------------------------------------------------------------------";
    int line_size = SUDOKU_GRID_SIZE + (SUDOKU_GRID_SIZE - 1) + 2;
    window_addch(window, ACS_LLCORNER);
    for (int k = 0; k < SUDOKU_GRID_SIZE; k++)
    {
        if (k == SUDOKU_GRID_SIZE - 1)
            window_printw(window, "%.*s", line_size, line);
        else
            window_printw(window, "%.*s+", line_size, line);
    }
    window_addch(window, ACS_LRCORNER);
    window_printw(window, "\n");
}

void print_line(window_t *window)
{
    char *line = "-------------------------------------------------------------------------";
    int line_size = SUDOKU_GRID_SIZE + (SUDOKU_GRID_SIZE - 1) + 2;
    window_addch(window, ACS_LTEE);
    for (int k = 0; k < SUDOKU_GRID_SIZE; k++)
    {
        if (k == SUDOKU_GRID_SIZE - 1)
            window_printw(window, "%.*s", line_size, line);
        else
            window_printw(window, "%.*s+", line_size, line);
    }
    window_addch(window, ACS_RTEE);
    window_printw(window, "\n");
}

void show_sudoku_grid(int sudoku[][SUDOKU_SIZE], window_t *window)
{
    int i, j;

    window_erase(window);
    print_uline(window);
    for (i = 0; i < SUDOKU_SIZE; i++)
    {
        for (j = 0; j < SUDOKU_SIZE; j++)
        {
            if (j == 0)
                window_printw(window, "| ");
            window_printw(window, "%d ", sudoku[i][j]);
            if ((j + 1) % 3 == 0)
                window_printw(window, "| ");
        }
        window_printw(window, "\n");
        if ((i + 1) % 3 == 0 && i != SUDOKU_SIZE - 1)
            print_line(window);
    }
    print_bline(window);
    window_refresh(window);
}

int main()
{
    int fd;

    // create and open data visualization pipe
    if (mkfifo(PIPE_NAME, S_IRUSR | S_IWUSR) == -1)
    {
        fprintf(stderr, "Error creating named pipe '%s'", PIPE_NAME);
        perror("");
        exit(EXIT_FAILURE);
    }

    size_t bytes, bytes_read = 0;
    int sudoku_buffer[SUDOKU_SIZE][SUDOKU_SIZE];
    memset(sudoku_buffer, 0, (PUZZLE_SIZE * sizeof(int)));
    int pipe_flag;

    printf("================================================\n");
    printf("Sudoku Solving Visualization program\n");
    printf("================================================\n");

    printf("Waiting for sudoku to start...\n");

    ncurses_init();
    //
    int y, x;
    getmaxyx(stdscr, y, x);
    window_t *winfo = window_create(0, 0, x - 1, 6, "Informations", FALSE);
    window_mvprintw(winfo, 0, 0, "=========================================");
    window_mvprintw(winfo, 1, 0, "  Sudoku Solving Visualization program");
    window_mvprintw(winfo, 2, 0, "=========================================");
    window_mvprintw(winfo, 3, 0, "Bytes read: %ld", bytes_read);
    window_refresh(winfo);
    //
    window_t * wsudoku = window_create(0, 6, x-1, y-6, "Sudoku", FALSE);
    window_refresh(wsudoku);
    //
    show_sudoku_grid(sudoku_buffer, wsudoku);

    if ((fd = open(PIPE_NAME, O_RDONLY)) == -1)
    {
        perror("Error opening pipe");
        exit(EXIT_FAILURE);
    }

    while (true)
    {
        if ((bytes = read(fd, &pipe_flag, sizeof(int))) == -1)
        {
            perror("Error reading named pipe flag from pipe");
            exit(EXIT_FAILURE);
        }
        bytes_read += bytes;
        //window_refresh(winfo);
        for (int s = 0; s < SUDOKU_SIZE; s++)
        {
            for (int h = 0; h < SUDOKU_SIZE; h++)
            {
                if ((bytes = read(fd, &sudoku_buffer[s][h], sizeof(int))) == -1)
                {
                    perror("Error reading sudoku data from pipe");
                    exit(EXIT_FAILURE);
                }
            }
            bytes_read += bytes;
            //window_refresh(winfo);
        }
        window_mvprintw(winfo, 3, 0, "Bytes read: %ld", bytes_read);

        show_sudoku_grid(sudoku_buffer, wsudoku);

        if (pipe_flag == PIPE_CLOSE)
        {
            break;
        }
    }

    if (close(fd) == -1)
    {
        perror("Error closing pipe");
        exit(EXIT_FAILURE);
    }

    if (unlink(PIPE_NAME) == -1)
    {
        perror("Error deleting pipe");
        exit(EXIT_FAILURE);
    }

    window_delete(&winfo);
    window_delete(&wsudoku);
    ncurses_stop();

    printf("End of program\n");

    return EXIT_SUCCESS;
}
