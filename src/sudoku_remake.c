#include <math.h>
#include <omp.h>

#include <stdio.h>

#include "sudoku.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Use: %s Flags file puzzle\n", argv[0]);
        fprintf(stderr, "Where :\n");
        fprintf(stderr, "  file   : The file containing the sudoku puzzles\n");
        fprintf(stderr, "  puzzle : The hash of the puzzle to solve\n");
        fprintf(stderr, "Flags :\n");
        fprintf(stderr, "  -v : Program verbose output\n");
        exit(EXIT_FAILURE);
    }

    // retrieve the starting grid from the sudoku file
    bool verbose = (argc == 4) ? true : false;
    char *puzzle_hash = (argc == 3) ? argv[2] : argv[3];
    char *file = (argc == 3) ? argv[1] : argv[2];
    char filename[FILE_SIZE] = SUDOKU_DIR;
    strcat(filename, file);

    printf("%s#File currently being solved [%s]%s\n", CLR_GRN, puzzle_hash, CLR_RESET);

    // print the current solving configuration
    if (PRINT_CONFIG)
        print_config();

    int **original_grid = read_sudoku_file(filename, SUDOKU_SIZE, puzzle_hash);

    // seperate the region, lines and columns of the grid into 3 variables
    int **lines;
    int ***columns;
    int ***regions;
    // make a deep copy of the original grid with lines and have the regions and columns point to it
    lines = create_sudoku_lines(original_grid);
    // create a 2D array of pointers where each array is a region of the grid(from left to right)
    regions = create_sudoku_region(lines);
    // print_sudoku_pointers(regions);
    columns = create_sudoku_columns(lines);

    int tries;
    int cost = (int)INFINITY;
    double CPU_time, start_time, end_time;
    int lowest_cost_found = (int)INFINITY;
    int **best_solution = NULL;
    setting_t setting;

    FILE *fd = NULL;
    FILE *gnuplotPipe;
    pid_t pid = 0;

    char date_buffer[FILE_SIZE];
    time_t timestamp = time(NULL);
    strftime(date_buffer, FILE_SIZE, "%d-%m-%Y-(%H-%M-%S)", localtime(&timestamp));

    if (GET_STATS)
    {
        // Remove the existing plot.dat file, if any.
        if (remove("plot.dat") == 0)
        {
            printf("plot.dat removed\n");
        }
        fd = fopen("plot.dat", "w");
        if (fd == NULL)
        {
            perror("Failed to open plot.dat");
            exit(1);
        }
        fclose(fd);
        fd = fopen("plot.dat", "a");
        setting.stat_file = fd;

        gnuplotPipe = popen("gnuplot", "w");
        if (gnuplotPipe == NULL)
        {
            perror("Error opening Gnuplot pipe\n");
            return 1;
        }
    }

#if _SHOW_
    int filed;
    if ((filed = open(PIPE_NAME, O_WRONLY)) == -1)
    {
        perror("Error opening pipe");
        exit(EXIT_FAILURE);
    }
    setting.fd = filed;
#endif

    start_time = omp_get_wtime();

    // Start of the solving phase until we get SOLUTION_COST value
    if (GET_STATS)
        pid = fork();
    if (pid == 0)
    {
        if (KEEP_START)
        {
            sudoku_copy_content(&lines, original_grid);
        }

        tries = 0;
        while (cost != SOLUTION_COST && tries <= MAX_TRIES)
        {
            simmulated_annealing(original_grid, lines, columns, regions, &cost, setting);
            tries++;
            if (verbose)
            {
                printf("\n===========================\n");
                print_sudoku(lines);
                printf("[TRY#%d]>> Current cost : %d\n", tries, cost);
            }
            // log the stats of the recuit solver
            // if (GET_STATS)
            //    sudoku_write_stats(puzzle_hash, cost, tries, date_buffer);

            // find lowest cost and manage the best current solution
            if (cost < lowest_cost_found)
            { // if we find the current best solution, keep the cost and the grid
                lowest_cost_found = cost;
                if (KEEP_BEST)
                {
                    if (best_solution != NULL)
                    {
                        sudoku_free(best_solution);
                        best_solution = NULL;
                    }
                    best_solution = create_sudoku_lines(lines);
                }
            }
            else if (KEEP_BEST)
            { // if the cost found is inferior, go back to best solution
                sudoku_copy_content(&lines, best_solution);
            }
        }
        if (GET_STATS)
            exit(EXIT_SUCCESS);
    }
    else
    {
        if (GET_STATS)
        {
            fprintf(gnuplotPipe, "load 'liveplot_cost.gnu'\n");
            fflush(gnuplotPipe);
        }
    }
    end_time = omp_get_wtime();

    if (GET_STATS)
    {
        pclose(gnuplotPipe);
        fclose(fd);
    }

#if _SHOW_
    // send pipe closing signal to process
    sudoku_pipe_close(filed, PIPE_CLOSE);
#endif

    // calculate the CPU execution time of the sudoku solving algorithm
    CPU_time = end_time - start_time;

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    if (verbose)
    {
        printf("\n===========================\n");
        printf("\nResults of the simulation: \n");
        printf("\n===========================\n");

        printf("\n---------------------------------------------------------------------------------\n");
    }

    if (verbose)
    {
        printf("\n===========================\n");
        printf("From: ");
        printf("\n===========================\n");
        print_sudoku(original_grid);
    }

    if (verbose)
    {
        printf("\n===========================\n");
        printf("To: ");
        printf("\n===========================\n");
        print_sudoku(lines);
    }

    printf(">> Current cost at the end of the simulation : %d\n", cost);
    printf(">> Best solution (lowest cost) found during the execution of the simulation : %d\n", lowest_cost_found);
    printf(">> Numbers of tries taken : %d\n", tries);
    printf(">> CPU Execution time of the sudoku solving simulation : %f\n", CPU_time);

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    // Free allocated memory of the various grids used
    sudoku_free(original_grid);
    sudoku_free_pointers(regions);
    sudoku_free_pointers(columns);
    sudoku_free(lines);
    if (best_solution != NULL)
        sudoku_free(best_solution);

    return EXIT_SUCCESS;
}