#include <math.h>
#include <omp.h>

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

    int **original_grid = read_sudoku_file(filename, SUDOKU_SIZE, puzzle_hash);
    // seperate the region, lines and columns of the grid into 3 variables
    int **lines;
    int ***columns;
    int ***regions;
    int tries;
    int cost = (int)INFINITY;
    double CPU_time, start_time, end_time;

    start_time = omp_get_wtime();
    for (int i = 0; i < 10; i++)
    {
        // make a deep copy of the original grid with lines and have the regions and columns point to it
        lines = create_sudoku_lines(original_grid);
        // create a 2D array of pointers where each array is a region of the grid(from left to right)
        regions = create_sudoku_region(lines);
        // print_sudoku_pointers(regions);
        columns = create_sudoku_columns(lines);
        tries = 0;
        while (cost != 0)
        {
            simmulated_annealing(original_grid, lines, columns, regions, &cost);
            tries++;
            if (verbose)
            {
                printf("\n===========================\n");
                print_sudoku(lines);
                printf("[TRY#%d]>> Current cost : %d\n", tries, cost);
            }
        }

        printf("Essai %d\n", i);

        if (cost == 0)
            break;
        sudoku_free_pointers(regions);
        sudoku_free_pointers(columns);
        sudoku_free(lines);
    }
    end_time = omp_get_wtime();

    // calculate the CPU execution time of the sudoku solving algorithm
    CPU_time = end_time - start_time;

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

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
    printf(">> Numbers of tries taken : %d\n", tries);
    printf(">> CPU Execution time of the sudoku solving simulation : %f\n", CPU_time);

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    // Free allocated memory of the various grids used
    sudoku_free(original_grid);
    if (cost == 0)
    {
        sudoku_free_pointers(regions);
        sudoku_free_pointers(columns);
        sudoku_free(lines);
    }

    return EXIT_SUCCESS;
}