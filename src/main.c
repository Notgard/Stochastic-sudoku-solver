#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <sys/stat.h>

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
int sudoku_constraints(int **original_grid, int **lines, int ***columns, int ***regions)
{
    int sum = 0;
#if _DEBUG_
    int constraints[9][9];
    for (int i = 0; i < SUDOKU_SIZE; i++)
    {
        for (int j = 0; j < SUDOKU_SIZE; j++)
        {
            constraints[i][j] = 0;
        }
    }
#endif
    for (int i = 0; i < SUDOKU_SIZE; i++)
    {
        for (int j = 0; j < SUDOKU_SIZE; j++)
        {
            if (original_grid[i][j] == 0)
            {
                sum += sudoku_cell_constraints(lines[i][j], i, j, lines, columns, regions);
#if _DEBUG_
                constraints[i][j] = sudoku_cell_constraints(lines[i][j], i, j, lines, columns, regions);
#endif
            }
        }
    }
#if _DEBUG_
    printf("-------------------[\n");
    print_sudoku_grid(constraints);
    printf("]-------------------\n");
#endif
    return sum;
    // return sum / 2;
}

int sudoku_constraints_old(int **original_grid, int **lines, int ***columns, int ***regions)
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
    char *file = (argc == 3) ? argv[1] : argv[2];
    char filename[FILE_SIZE] = SUDOKU_DIR;
    strcat(filename, file);

    printf("%s#File currently being solved [%s]%s\n", CLR_GRN, puzzle_hash, CLR_RESET);
    printf("%s#Maximum tries : %s[%d]\n", CLR_GRN, CLR_RESET, MAX_TRIES);

    // print the current solving configuration
    if (PRINT_CONFIG)
        print_config();

    int **original_grid = read_sudoku_file(filename, SUDOKU_SIZE, puzzle_hash);
    // seperate the region, lines and columns of the grid into 3 variables

    // make a deep copy of the original grid with lines and have the regions and columns point to it
    int **lines = create_sudoku_lines(original_grid);

    // create a 2D array of pointers where each array is a region of the grid(from left to right)
    int ***regions = create_sudoku_region(lines);
    // print_sudoku_pointers(regions);
    int ***columns = create_sudoku_columns(lines);
    // print_sudoku_pointers(columns);

    int cost = 0;
    if(OLD) cost = sudoku_constraints_old(original_grid, lines, columns, regions);
    else cost = sudoku_constraints(original_grid, lines, columns, regions);

    if(verbose)
        printf(">> Current cost : %d\n", cost);

#if _SHOW_
    // create and open data visualization pipe
    if (mkfifo(DATA_PIPE, S_IRUSR | S_IWUSR) == -1)
    {
        fprintf(stderr, "Error creating named pipe '%s'", PIPE_NAME);
        perror("");
        exit(EXIT_FAILURE);
    }

    if ((fd = open(PIPE_NAME, O_WRONLY)) == -1)
    {
        perror("Error opening pipe");
        exit(EXIT_FAILURE);
    }
#endif

    //  randomly all of the cells of the grid with values from 1 to 9, except the ones already placed
    unsigned int seed = (unsigned int)time(NULL);

    if (!RANDOMIZE_SUDOKU)
    { // randomize the sudoku only once at the start
        // Step 1: Fill the grid's non fixed cells with random values and calculate the cost of the grid
        sudoku_randomize(&lines, original_grid, &seed);
        // calculate cost of the random grid
        if (OLD)
            cost = sudoku_constraints_old(original_grid, lines, columns, regions);
        else
            cost = sudoku_constraints(original_grid, lines, columns, regions);
    }

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
    int k, cost_one, cost_two, cost_comp, temp, new;
    int lowest_cost_found = (int)INFINITY;
    double start_time, end_time, CPU_time;
    double u;
    int **best_solution = NULL;
    //
    start_time = omp_get_wtime();
    tries = 0;

    while (solved != true)
    {
        if (KEEP_START)
        {
            sudoku_copy_content(&lines, original_grid);
        }

        if (verbose)
        {
            printf("\n===========================\n");
            print_sudoku(lines);
            printf(">> Current cost : %d\n", cost);
        }

        if (RANDOMIZE_SUDOKU)
        { // randomize only on the first try
            // Step 1: Fill the grid's non fixed cells with random values and calculate the cost of the grid
            sudoku_randomize(&lines, original_grid, &seed);
            if (OLD)
                cost = sudoku_constraints_old(original_grid, lines, columns, regions);
            else
                cost = sudoku_constraints(original_grid, lines, columns, regions);
            //print_sudoku(lines);
            //printf("Cost after randomization : %d\n", cost);
        }

        if (verbose)
        {
            printf("\n===========================\n");
            print_sudoku(lines);
            printf(">> Current cost : %d\n", cost);
        }

        // log the stats of the recuit solver
        if (GET_STATS)
            sudoku_write_stats(puzzle_hash, cost, tries, date_buffer);

        // Step 2: Setup the contants
        int i = -1, j = -1;
        float sigma = 0.1;
        double ep = START_TEMPERATURE;
        double e = exp(1);
        double temperature = ep;

        // diminution/augmentation de la temperature de départ à chaque quart d'essaie
        if (tries != 0 && tries % (MAX_TRIES / TEMP_STEP) == 0)
            temperature *= 2;

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
                while ((new = get_bound_random(&seed, 1, 9)) == temp)
                    ;
                lines[i][j] = new;

                // Step 7: evaluate the cost of the random cell with changed value
                cost_two = sudoku_cell_constraints(lines[i][j], i, j, lines, columns, regions);

                // Step 8: compare the cost between the two random cell values
                cost_comp = cost - cost_one + cost_two;

                // Step 9: choose random value in [0, 1]
                u = get_random(&seed);
#if _DEBUG_
                snprintf(debug_buffer, DEBUG_SIZE, "{cost: %d, cost_one: %d, cost_two: %d, cost_comp: %d, temperature: %f}", cost, cost_one, cost_two, cost_comp, temperature);
                sudoku_debug_output(puzzle_hash, debug_buffer, date_buffer);
#endif
                // Step 10: probability acceptance
                if (cost_comp < cost)
                {
                    cost = cost_comp;
                }
                //else if (u <= MIN(1, e - ((cost_comp - cost) / temperature)))
                else if (u <= exp(-((cost_comp - cost) / temperature)))
                { // acceptation
                    cost = cost_comp;
                }
                else
                { // rejet
                    lines[i][j] = temp;
                }

// send current sudoku to visualization program
#if _SHOW_
                for (int s = 0; s < SUDOKU_SIZE; s++)
                {
                    if (write(fd, &lines[s], sizeof(int) * SUDOKU_SIZE) == -1)
                    {
                        perror("Error writing integers in pipe");
                        exit(EXIT_FAILURE);
                    }
                }
#endif
                // Stop the algorithm if the cost of the grid is SOLUTION_COST
                if (cost <= SOLUTION_COST)
                {
                    printf("\n>>> [NULL 0 cost solution found]\n");
                    print_sudoku(lines);
                    solved = true;
                    // log the stats of the recuit solver
                    if (GET_STATS)
                        sudoku_write_stats(puzzle_hash, cost, tries, date_buffer);
                    break;
                }
#if _DEBUG_
                snprintf(debug_buffer, DEBUG_SIZE, "{cost: %d, cost_one: %d, cost_two: %d, cost_comp: %d, temperature: %f}", cost, cost_one, cost_two, cost_comp, temperature);
                sudoku_debug_output(puzzle_hash, debug_buffer, date_buffer);
#endif
            }

            // Step k: reduce the temperature
            temperature = temperature / (1 + (log(1 + sigma) / ep + 1) * temperature);
        }

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

        // increment the number of tries
        tries++;
        if (tries > MAX_TRIES && !KEEP_TRYING)
            break;
    }

#if _SHOW_
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
#endif

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
        printf(">>> Last output cost by the annealing algorithm: %d\n", cost);
    }

    // calculate cost of grid
    if (OLD)
        cost = sudoku_constraints_old(original_grid, lines, columns, regions);
    else
        cost = sudoku_constraints(original_grid, lines, columns, regions);

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
    printf(">> Numbers of tries taken : %d\n", tries - 1);
    printf(">> CPU Execution time of the sudoku solving simulation : %f\n", CPU_time);

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    // Free allocated memory of the various grids used
    sudoku_free_pointers(regions);
    sudoku_free_pointers(columns);
    sudoku_free(lines);
    sudoku_free(original_grid);
    if (best_solution != NULL)
        sudoku_free(best_solution);

    return EXIT_SUCCESS;
}