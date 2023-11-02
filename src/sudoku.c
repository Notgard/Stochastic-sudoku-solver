#include "sudoku.h"

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
    int cost;
    for (int i = 0; i < SUDOKU_SIZE; i++)
    {
        for (int j = 0; j < SUDOKU_SIZE; j++)
        {
            if (original_grid[i][j] == 0)
            {
                cost = sudoku_cell_constraints(lines[i][j], i, j, lines, columns, regions);
                if (cost < 0)
                {
                    fprintf(stderr, "ERROR: negative cell cost %d !!\n", cost);
                }
                sum += cost;
            }
        }
    }
    return sum;
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

void simmulated_annealing(int **original_grid, int **lines, int ***columns, int ***regions, int *cost)
{
    unsigned int seed = (unsigned int)time(NULL);
    double delta = 0.1;
    double e_p = START_TEMPERATURE;
    double temperature = e_p;
    int line = get_bound_random(&seed, 0, 8);
    int column = get_bound_random(&seed, 0, 8);
    int temp = 0;
    int cost1 = 0, cost2 = 0;
    int new_val;
    int diff_cost = 0;
    double u;
    bool solved = false;

    // Step 1: Fill the grid's non fixed cells with random values and calculate the cost of the grid
    sudoku_randomize(&lines, original_grid, &seed);

    int total_cost = sudoku_constraints_old(original_grid, lines, columns, regions);

    // Step 3: Start the recuit simulation algorithm
    while (temperature >= TEMPERATURE_CEILING)
    {
        for (int i = 0; i < PRESUMED_PUZZLE_SIZE; i++)
        {
            // Step 4: choose random cell from the grid which isn't fixed
            sudoku_get_random_cell(original_grid, &line, &column, &seed);

            // Step 5: store the value in a temp variable and evaluate the cost of the random cell
            temp = lines[line][column];
            cost1 = sudoku_cell_constraints(lines[line][column], line, column, lines, columns, regions);

            // Step 6: choose a new different value for the random cell
            while ((new_val = get_bound_random(&seed, 1, 9)) == temp);

            lines[line][column] = new_val;
            // Step 7: evaluate the cost of the random cell with changed value
            cost2 = sudoku_cell_constraints(lines[line][column], line, column, lines, columns, regions);

            // Step 8: compare the cost between the two random cell values
            diff_cost = total_cost - cost1 + cost2;
            
            // Step 9: choose random value in [0, 1]
            u = get_random(&seed);
            // Step 10: probability acceptance
            if (diff_cost < total_cost)
            { // acceptation
                total_cost = diff_cost;
            }
            else if (u <= MIN(1, exp(-((diff_cost - total_cost) / temperature))))
            { // acceptation probabilitaire
                total_cost = diff_cost;
            }
            else
            { // rejet
                lines[line][column] = temp;
            }

            if (total_cost == 0)
            {
                solved = true;
                break;
            }
        }
        if(solved) break;
        temperature = temperature / (1 + (log(1 + delta) / (e_p + 1)) * temperature);
    }
    *cost = total_cost;
}