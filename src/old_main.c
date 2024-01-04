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
    if (OLD)
        cost = sudoku_constraints_old(original_grid, lines, columns, regions);
    else
        cost = sudoku_constraints(original_grid, lines, columns, regions);

    if (verbose)
        printf(">> Current cost : %d\n", cost);

#if _SHOW_
    int fd;

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
#if _SHOW_
    int pipe_flag = PIPE_OPEN;
#endif
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
            else if (!OLD)
                cost = sudoku_constraints(original_grid, lines, columns, regions);
            // print_sudoku(lines);
            // printf("Cost after randomization : %d\n", cost);
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
        // double e = exp(1);
        double temperature = ep;

        // diminution/augmentation de la temperature de départ à chaque quart d'essaie
        if (tries != 0 && tries % (MAX_TRIES / TEMP_STEP) == 0)
            temperature /= 2;

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
                // snprintf(debug_buffer, DEBUG_SIZE, "{cost: %d, cost_one: %d, cost_two: %d, cost_comp: %d, temperature: %f}", cost, cost_one, cost_two, cost_comp, temperature);
                // sudoku_debug_output(puzzle_hash, debug_buffer, date_buffer);
#endif
                // Step 10: probability acceptance
                if (cost_comp < cost)
                {
                    cost = cost_comp;
                }
                // else if (u <= MIN(1, e - ((cost_comp - cost) / temperature)))
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
                sudoku_pipe_write(fd, pipe_flag, lines);
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
    // send pipe closing signal to process
    pipe_flag = PIPE_CLOSE;
    sudoku_pipe_close(fd, pipe_flag);
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
    }
    printf(">>> Last output cost by the annealing algorithm: %d\n", cost);

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