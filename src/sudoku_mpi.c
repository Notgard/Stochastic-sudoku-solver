#include <math.h>
#include <omp.h>
#include <mpi.h>

#include <stdio.h>

#include "sudoku.h"

#define MASTER 0

int total_tries = 0;

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

    int id, p;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    int **original_grid;

    bool verbose;
    char *puzzle_hash;
    char *file;

    // retrieve the starting grid from the sudoku file
    verbose = (argc == 4) ? true : false;
    puzzle_hash = (argc == 3) ? argv[2] : argv[3];
    file = (argc == 3) ? argv[1] : argv[2];
    char filename[FILE_SIZE] = SUDOKU_DIR;
    strcat(filename, file);

    if(id == MASTER) {
        printf("%s#File currently being solved [%s]%s\n", CLR_GRN, puzzle_hash, CLR_RESET);

        // print the current solving configuration
        if (PRINT_CONFIG)
            print_config();
    }

    original_grid = read_sudoku_file(filename, SUDOKU_SIZE, puzzle_hash);

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

    int cost = (int)INFINITY;
    double CPU_time, start_time = 0.0, end_time = 0.0;
    int lowest_cost_found = (int)INFINITY;
    int **best_solution = NULL;
    setting_t setting;

    char date_buffer[FILE_SIZE];
    if(id == MASTER) {
        //get date for data file
        time_t timestamp = time(NULL);
        strftime(date_buffer, FILE_SIZE, "%d-%m-%Y-(%H-%M-%S)", localtime(&timestamp));
        
        //record algorithm start time
        start_time = omp_get_wtime();
    }

    int tries_per_process = MAX_TRIES / p;
    int local_tries = 0;
    int local_cost = cost;

    // Start of the solving phase until we get SOLUTION_COST value
    while (cost != SOLUTION_COST && local_tries <= tries_per_process)
    {
        if (KEEP_START)
        {
            sudoku_copy_content(&lines, original_grid);
        }

        #if _PARALLEL_
            parallel_simmulated_annealing_test(original_grid, lines, columns, regions, &local_cost, setting);
        #else
            simmulated_annealing(original_grid, lines, columns, regions, &local_cost, setting);
        #endif
        
        local_tries++;
        total_tries++;

        if (verbose)
        {
            printf("\n===========================\n");
            print_sudoku(lines);
            printf("[PROCESS #%d | TRY#%d]>> Current cost : %d\n", id, total_tries, local_cost);
        }

        MPI_Allreduce(&local_cost, &cost, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);

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

    if(id == MASTER) {
        end_time = omp_get_wtime();

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
        printf(">> Numbers of tries taken : %d\n", total_tries);
        printf(">> CPU Execution time of the sudoku solving simulation : %f\n", CPU_time);
    }

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    MPI_Finalize();

    // Free allocated memory of the various grids used
    sudoku_free(original_grid);
    sudoku_free_pointers(regions);
    sudoku_free_pointers(columns);
    sudoku_free(lines);
    if (best_solution != NULL)
        sudoku_free(best_solution);

    return EXIT_SUCCESS;
}