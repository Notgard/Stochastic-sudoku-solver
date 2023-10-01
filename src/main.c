#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <locale.h>

#include <math.h>
#include <omp.h>

#include "utils.h"

/// @brief Returns a 2D array containing each region in order (from left to right)
/// @param sudoku_grid
/// @return An integer array containing every region of the sudoku grid in a 1D array for each region block
int ***create_sudoku_region(int **sudoku_grid)
{
    int ***puzzle_grid;

    int lines = SUDOKU_SIZE, cols = SUDOKU_SIZE;

    if ((puzzle_grid = (int ***)malloc(sizeof(int **) * lines)) == NULL)
    {
        fprintf(stderr, "ERROR: Out of memory!!\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < lines; i++)
    {
        if ((puzzle_grid[i] = (int **)malloc(cols * sizeof(int *))) == NULL)
        {
            fprintf(stderr, "ERROR: Out of memory!!\n");
            exit(EXIT_FAILURE);
        }
    }

    int i, j, k, cursor;
    int i_offset = 0, j_offset = 0;
    for (k = 0; k < SUDOKU_SIZE; k++, j_offset += 3)
    {
        cursor = 0;

        if (k != 0 && k % 3 == 0)
        {
            i_offset += 3;
            j_offset = 0;
        }

        for (i = 0; i < SUDOKU_SIZE / 3; i++)
        {
            for (j = 0; j < SUDOKU_SIZE / 3; j++)
            {
                puzzle_grid[k][cursor] = &sudoku_grid[i + i_offset][j + j_offset];
                cursor++;
            }
        }
    }

    return puzzle_grid;
}

/// @brief Returns a 2D array containing each line of the sudoku grid in order (from top to bottom)
/// Each value points to the values in the provided array
/// @param sudoku_grid the provided array
/// @return
int **create_sudoku_lines(int **sudoku_grid)
{
    int **puzzle_grid;

    int lines = SUDOKU_SIZE, cols = SUDOKU_SIZE;

    if ((puzzle_grid = (int **)malloc(sizeof(int *) * lines)) == NULL)
    {
        fprintf(stderr, "ERROR: Out of memory!!\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < lines; i++)
    {
        if ((puzzle_grid[i] = (int *)malloc(cols * sizeof(int))) == NULL)
        {
            fprintf(stderr, "ERROR: Out of memory!!\n");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < SUDOKU_SIZE; i++)
    {
        for (int j = 0; j < SUDOKU_SIZE; j++)
        {
            puzzle_grid[i][j] = sudoku_grid[i][j];
        }
    }

    return puzzle_grid;
}

/// @brief Returns a 2D array containing each line of the sudoku grid in order (from left to right)
/// Each value points to the values in the provided array
/// @param sudoku_grid the provided array
/// @return
int ***create_sudoku_columns(int **sudoku_grid)
{
    int ***puzzle_grid;

    int lines = SUDOKU_SIZE, cols = SUDOKU_SIZE;

    if ((puzzle_grid = (int ***)malloc(sizeof(int **) * lines)) == NULL)
    {
        fprintf(stderr, "ERROR: Out of memory!!\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < lines; i++)
    {
        if ((puzzle_grid[i] = (int **)malloc(cols * sizeof(int *))) == NULL)
        {
            fprintf(stderr, "ERROR: Out of memory!!\n");
            exit(EXIT_FAILURE);
        }
    }

    int i, j;
    for (i = 0; i < SUDOKU_SIZE; i++)
    {
        for (j = 0; j < SUDOKU_SIZE; j++)
        {
            puzzle_grid[i][j] = &sudoku_grid[j][i];
        }
    }

    return puzzle_grid;
}

/**
 * @brief Frees memory from each line and the sudoku grid itself
 *
 * @param sudoku_grid
 */
void sudoku_free(int **sudoku_grid)
{
    if (sudoku_grid != NULL)
    {
        // Free dynamically allocated memory
        for (int i = 0; i < SUDOKU_SIZE; i++)
        {
            free(sudoku_grid[i]);
        }
        free(sudoku_grid);
    }
}

/**
 * @brief Frees memory from each line and the sudoku grid itself
 *
 * @param sudoku_grid
 */
void sudoku_free_pointers(int ***sudoku_grid)
{
    if (sudoku_grid != NULL)
    {
        // Free dynamically allocated memory
        for (int i = 0; i < SUDOKU_SIZE; i++)
        {
            free(sudoku_grid[i]);
        }
        free(sudoku_grid);
    }
}

/// @brief Calculates the total amount of constraints violated in the sudoku grid by checking
///        the occurence of a given number in the lines, columns and regions of the sudoku
/// @param nb the given number to find the total amount of constraints violated
/// @param start_l the starting index of the line
/// @param start_c the starting index of the column
/// @param lines 2D array containing each line of the sudoku grid
/// @param columns 2D array containing each column of the sudoku grid
/// @param regions 2D array containing each region of the sudoku grid
/// @return the total amount of constraints violated in the given sudoku for the given cell
int sudoku_cell_constraints(int nb, int start_l, int start_c, int **lines, int ***columns, int ***regions)
{
    if (nb == 0)
        return 0;
    int sum = 0;
    int start_r = (start_l / 3) * 3 + (start_c / 3); // gets the specific region to check inside
    for (int j = 0; j < SUDOKU_SIZE; j++)
    {
        // if (lines[start_l][j] == nb || (*columns[start_c][j]) == nb || (*regions[start_r][j]) == nb)
        //{ // check the content of the line, column and region
        //     sum++;
        // }
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

/// @brief Checks the total amount of constraints violated of every cell in the provided grid
/// @param lines 2D array containing each line of the sudoku grid
/// @param columns 2D array containing each column of the sudoku grid
/// @param regions 2D array containing each region of the sudoku grid
/// @return the total amount of constraints violated in the given sudoku
int sudoku_constraints(int **lines, int ***columns, int ***regions)
{
    int sum = 0;
    for (int i = 0; i < SUDOKU_SIZE; i++)
    {
        for (int j = 0; j < SUDOKU_SIZE; j++)
        {
            sum += sudoku_cell_constraints(lines[i][j], i, j, lines, columns, regions);
        }
    }
    return sum / 2;
}

/// @brief Chooses a random cell from the sudoku grid. If i or j != -1 then the random cell chosen needs to be different
///        from the previous cell chosen by the function. This is done to avoid repeated randomly chosen cells
/// @param sudoku_grid the provided sudoku grid
/// @param i the line index
/// @param j the column index
void sudoku_get_random_cell(int **sudoku_grid, int *i, int *j, unsigned int *seed)
{
    int line = get_bound_random(seed, 0, 8);
    int col = get_bound_random(seed, 0, 8);

    bool different = (*i != -1 && *j != -1);
    while (sudoku_grid[line][col] != 0 || different)
    {
        line = get_bound_random(seed, 0, 8);
        col = get_bound_random(seed, 0, 8);
        if (different && (line != *i || col != *j))
        {
            different = false;
        }
    }

    *i = line;
    *j = col;
}

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
    char * file = (argc == 3) ? argv[1] : argv[2];
    char filename[FILE_SIZE] = SUDOKU_DIR;
    strcat(filename, file);

    printf("%s#File currently being solved [%s]%s\n", CLR_GRN, puzzle_hash, CLR_RESET);
    printf("%s#Maximum tries : %s[%d]\n", CLR_GRN, CLR_RESET, MAX_TRIES);

    int **original_grid = read_sudoku_file(filename, SUDOKU_SIZE, puzzle_hash);
    if(verbose)
        print_sudoku(original_grid);
    // seperate the region, lines and columns of the grid into 3 variables

    // make a deep copy of the original grid with lines and have the regions and columns point to it
    int **lines = create_sudoku_lines(original_grid);

    // create a 2D array of pointers where each array is a region of the grid(from left to right)
    int ***regions = create_sudoku_region(lines);
    // print_sudoku_pointers(regions);
    int ***columns = create_sudoku_columns(lines);
    // print_sudoku_pointers(columns);

    int cost = 0;
    cost = sudoku_constraints(lines, columns, regions);
    if(verbose)
        printf(">> Current cost : %d\n", cost);
    // tests for the sudoku cell constraints function on fixed cells
    // printf(">> cost %d: %d\n", lines[0][1], sudoku_cell_constraints(lines[0][1], 0, 1, lines, columns, regions));
    // printf(">> cost %d: %d\n", lines[1][2], sudoku_cell_constraints(lines[1][2], 1, 2, lines, columns, regions));
    // printf(">> cost %d: %d\n", lines[7][5], sudoku_cell_constraints(lines[7][5], 7, 5, lines, columns, regions));
    // printf(">> cost %d: %d\n", lines[7][1], sudoku_cell_constraints(lines[7][1], 7, 1, lines, columns, regions));
    // printf(">> cost %d: %d\n", lines[6][8], sudoku_cell_constraints(lines[6][8], 6, 8, lines, columns, regions));

//  randomly all of the cells of the grid with values from 1 to 9, except the ones already placed
    unsigned int seed = (unsigned int)time(NULL);


//

// Setup main loop and current timestamp
    int tries;
    char date_buffer[FILE_SIZE];
#if _DEBUG_
    char debug_buffer[DEBUG_SIZE];
#endif
    time_t timestamp = time(NULL);
    strftime(date_buffer, FILE_SIZE, "%d-%m-%Y-(%H-%M-%S)", localtime(&timestamp));
//

// define recuit algorithm variables
    bool solved = false;
    int k, u, cost_one, cost_two, cost_comp, temp, new;
    int lowest_cost_found = (int)INFINITY;
    double start_time, end_time, CPU_time;
//
    start_time = omp_get_wtime();
    //for (tries = 0; tries < MAX_TRIES; tries++)
    tries = 0;
    while(solved != true)
    {
        // Step 1: Fill the grid's non fixed cells with random values and calculate the cost of the grid
        sudoku_randomize(&lines, seed);
        // calculate cost of the random grid
        cost = sudoku_constraints(lines, columns, regions);
        if(verbose) {
            printf("\n===========================\n");
            print_sudoku(lines);
            printf(">> Current cost : %d\n", cost);
        }

        // log the stats of the recuit solver
        //sudoku_write_stats(puzzle_hash, cost, tries, date_buffer);

        // Step 2: Setup the contants
        int i = -1, j = -1;
        float sigma = 0.1;
        double ep = (1620 / 2) / (tries+1);
        double temperature = ep;
        double e = exp(1);

        // Step 3: Start the recuit simulation algorithm
        while (temperature >= TEMPERATURE_CEILING && solved != true)
        {
            for (k = 0; k < PRESUMED_PUZZLE_SIZE; k++)
            {
                // Step 4: choose random cell from the grid which isn't fixed
                sudoku_get_random_cell(original_grid, &i, &j, &seed); // use the original grid to find a non fixed random cell

                // Step 5: store the value in a temp variable and evaluate the cost of the random cell
                temp = lines[i][j];
                cost_one = sudoku_cell_constraints(lines[i][j], i, j, lines, columns, regions);

                // Step 6: choose a new different value for the random cell
                while ((new = get_bound_random(&seed, 1, 9)) == temp);
                lines[i][j] = new;

                // Step 7: evaluate the cost of the random cell with changed value
                cost_two = sudoku_cell_constraints(lines[i][j], i, j, lines, columns, regions);

                // Step 8: compare the cost between the two random cells
                cost_comp = cost - cost_one + cost_two;
                // Step 9: choose random value in [0, 1]
                u = get_random();
#if _DEBUG_ && 0
                if (tries == 1 && k < 2)
                {
                    snprintf(debug_buffer, DEBUG_SIZE, "\nFirst iterations of the second try of the algorithm ->{cost: %d, cost_one: %d, cost_two: %d, cost_comp: %d}\n", cost, cost_one, cost_two, cost_comp);
                    sudoku_debug_output(puzzle_hash, debug_buffer, date_buffer);
                }
#endif
                // Step 10: probability acceptance
                if (u <= MIN(1, e - ((cost_comp - cost) / temperature)))
                { // acceptation
                    cost = cost_comp;
                }
                else
                { // rejet
                    lines[i][j] = temp;
                }
                // Stop the k loop if the cost of the grid is 0
                if (cost == 0)
                {
                    if(verbose)
                        printf("\n>>> [NULL 0 cost solution found]\n");
                    solved = true;
                    break;
                }
            }
#if _DEBUG_
            snprintf(debug_buffer, DEBUG_SIZE, "{cost: %d, cost_one: %d, cost_two: %d, cost_comp: %d}", cost, cost_one, cost_two, cost_comp);
            sudoku_debug_output(puzzle_hash, debug_buffer, date_buffer);
#endif
            // Step k: reduce the temperature
            temperature = temperature / (1 + (log(1 + sigma) / ep + 1) * temperature);
            // temperature = 0.5 / 81 * log(9) - log(1 - sigma);
        }
        //find lowest cost and manage maximum amount of tries
        tries++;
        lowest_cost_found = (lowest_cost_found > cost) ? cost : lowest_cost_found;
        if(tries > MAX_TRIES) break;
    }
    end_time = omp_get_wtime();

    //calculate the CPU execution time of the sudoku solving algorithm
    CPU_time = end_time - start_time;

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    if(verbose) {
        printf("\n===========================\n");
        printf("\nResults of the simulation: \n");
        printf("\n===========================\n");

        printf("\n---------------------------------------------------------------------------------\n");
        printf(">>> Last output cost by the annealing algorithm: %d\n", cost);
    }

    // calculate cost of grid
    cost = sudoku_constraints(lines, columns, regions);

    if(verbose) {
        printf("\n===========================\n");
        printf("From: ");
        printf("\n===========================\n");

        print_sudoku(original_grid);
    }

    if(verbose) {
        printf("\n===========================\n");
        printf("To: ");
        printf("\n===========================\n");
    }
    print_sudoku(lines);
    printf(">> Current cost at the end of the simulation : %d\n", cost);
    printf(">> Best solution (lowest cost) found during the execution of the simulation : %d\n", lowest_cost_found);
    printf(">> CPU Execution time of the sudoku solving simulation : %f\n", CPU_time);

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    // Free allocated memory of the various grids used
    sudoku_free_pointers(regions);
    sudoku_free_pointers(columns);
    sudoku_free(lines);
    sudoku_free(original_grid);

    return EXIT_SUCCESS;
}