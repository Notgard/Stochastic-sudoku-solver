#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

#include <ncurses.h>

#include "config.h"
#include "sudoku.h"

#include "colors.h"
#include "functions.h"
#include "window.h"

#define ESC '\e'

#define COLORS 0

void signal_handler(int signum)
{
    if (unlink(PIPE_NAME) == -1)
    {
        perror("Error deleting pipe");
        exit(EXIT_FAILURE);
    }
}

void init_signal_handler()
{
    struct sigaction action;

    // Specify handler
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = signal_handler;
    if (sigaction(SIGINT, &action, NULL) == -1)
    {
        perror("Error positioning handler (2)");
        exit(EXIT_FAILURE);
    }
}

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

int cell_constraint(int nb, int start_l, int start_c, int **lines, int ***columns, int ***regions)
{
    if (nb == 0)
        return 0;
    int j;
    int sum = 0;
    int start_r = (start_l / 3) * 3 + (start_c / 3); // gets the specific region to check inside

    // #pragma omp parallel for num_threads(N_THREADS), reduction(+:sum), private(j), shared(start_r, start_c, start_l)
    for (j = 0; j < SUDOKU_SIZE; j++)
    {
        if (lines[start_l][j] == nb)
        {
            sum++;
        }
        if (*columns[start_c][j] == nb)
        {
            sum++;
        }
        if (*regions[start_r][j] == nb)
        {
            sum++;
        }
    }

    return sum - 3;
}

void show_sudoku_grid(int **sudoku, int ***regions, int ***columns, window_t *window)
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
            if (COLORS)
            {
                if (cell_constraint(sudoku[i][j], i, j, sudoku, columns, regions) > 0)
                {
                    window_printw_col(window, RED, "%d ", sudoku[i][j]);
                }
                else
                {
                    window_printw_col(window, GREEN, "%d ", sudoku[i][j]);
                }
            }
            else
            {
                window_printw(window, "%d ", sudoku[i][j]);
            }
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

    // prepare to close named pipe if process is killed
    init_signal_handler();

    // create and open data visualization pipe
    if (mkfifo(PIPE_NAME, S_IRUSR | S_IWUSR) == -1)
    {
        fprintf(stderr, "Error creating named pipe '%s'", PIPE_NAME);
        perror("");
        exit(EXIT_FAILURE);
    }

    size_t bytes;
    // setting up the data buffers
    cell_t cell;
    cell.cost = 0;
    int sudoku_buffer[SUDOKU_SIZE][SUDOKU_SIZE];
    int *buffer_pointer[SUDOKU_SIZE];
    memset(sudoku_buffer, 0, (PUZZLE_SIZE * sizeof(int)));
    for (int i = 0; i < SUDOKU_SIZE; i++)
    {
        buffer_pointer[i] = sudoku_buffer[i];
    }
    int ***regions = NULL;
    int ***columns = NULL;
    if (COLORS)
    {
        regions = create_sudoku_region(buffer_pointer);
        columns = create_sudoku_columns(buffer_pointer);
    }

    int pipe_flag;

    printf("================================================\n");
    printf("Sudoku Solving Visualization program\n");
    printf("================================================\n");

    printf("Waiting for sudoku to start...\n");

    ncurses_init();
    ncurses_colors(); 
    palette();
    clear();
    refresh();

    //
    int y, x;
    getmaxyx(stdscr, y, x);
    window_t *winfo = window_create(0, 0, x - 1, 6, "Informations", FALSE);
    window_mvprintw(winfo, 0, 0, "=========================================");
    window_mvprintw(winfo, 1, 0, "  Sudoku Solving Visualization program");
    window_mvprintw(winfo, 2, 0, "=========================================");
    window_mvprintw(winfo, 3, 0, "Current sudoku cost: %d", cell.cost);
    window_refresh(winfo);
    //
    window_t *wsudoku = window_create(0, 6, x - 1, y - 6, "Sudoku", FALSE);
    window_refresh(wsudoku);
    //
    show_sudoku_grid(buffer_pointer, regions, columns, wsudoku);

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

        switch (pipe_flag)
        {
        case PIPE_OPEN: // get the initial entire sudoku grid
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
            }
            break;
        case PIPE_CONTINUE: // only get the changed cell in the grid
            if ((bytes = read(fd, &cell, sizeof(cell_t))) == -1)
            {
                perror("Error reading cell data from pipe");
                exit(EXIT_FAILURE);
            }
            sudoku_buffer[cell.line][cell.col] = cell.val;
            window_erase(winfo);
            window_mvprintw(winfo, 0, 0, "==================================================");
            window_mvprintw(winfo, 1, 0, "      Sudoku Solving Visualization program");
            window_mvprintw(winfo, 2, 0, "==================================================");
            window_mvprintw(winfo, 3, 0, "Current sudoku cost: %d", cell.cost);
            window_refresh(winfo);
            break;
        case PIPE_CLOSE:
            goto exit_loop;
            break;
        }
        
        show_sudoku_grid(buffer_pointer, regions, columns, wsudoku);
    }
    exit_loop:;

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

    if (COLORS)
    {
        sudoku_free_pointers(regions);
        sudoku_free_pointers(columns);
    }

    printf("Final cost : %d\n", cell.cost);
    printf("End of program\n");

    return EXIT_SUCCESS;
}
