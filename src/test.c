#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <math.h>

#include "config.h"

#include "benchmark.h"

#define ROWS 3
#define COLS 4


#if 0

int** transpose2DArray(int** original, int rows, int cols) {
    int** transposed = (int**)malloc(cols * sizeof(int*));
    if (transposed == NULL) {
        fprintf(stderr, "ERROR: Out of memory!!\n");
        exit(EXIT_FAILURE);
    }

    // Create an array of pointers to original's columns
    for (int i = 0; i < cols; i++) {
        transposed[i] = (int*)malloc(sizeof(int*));
        if (transposed[i] == NULL) {
            fprintf(stderr, "ERROR: Out of memory!!\n");
            exit(EXIT_FAILURE);
        }
    }

    // Populate the transposed array with pointers to original columns
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            transposed[i][j] = &original[j][i];
        }
    }

    return transposed;
}

int test() {
    int** original = (int**)malloc(ROWS * sizeof(int*));
    for (int i = 0; i < ROWS; i++) {
        original[i] = (int*)malloc(COLS * sizeof(int));
    }

    int value = 1;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            original[i][j] = value++;
        }
    }

    int** transposed = transpose2DArray(original, ROWS, COLS);

    original[0][0] = 10;

    printf("Original array:\n");
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%d ", original[i][j]);
        }
        printf("\n");
    }

    printf("\nTransposed array:\n");
    for (int i = 0; i < COLS; i++) {
        for (int j = 0; j < ROWS; j++) {
            printf("%d ", *((int*)transposed[i][j])); // Dereference to get the value
        }
        printf("\n");
    }

    // Free dynamically allocated memory
    for (int i = 0; i < ROWS; i++) {
        free(original[i]);
    }
    free(original);

    for (int i = 0; i < COLS; i++) {
        free(transposed[i]);
    }
    free(transposed);

    return 0;
}
#endif

int get_bound_randomm(unsigned int *seed, unsigned int lBound, unsigned int uBound)
{
    int r = (int)rand_r(seed) % (uBound - lBound + 1) + lBound;
    return r;
}

int test(void) {
    unsigned int seed = 123456;

    printf("Series of 20 random numbers: \n");

    for(int i = 0; i < 20; i++) {
        printf("%d ", get_bound_randomm(&seed, 1, 10));
    }

    printf("\n");

    int rand_val;
    for(int i = 0; i < 20; i++) {
        rand_val = (int)rand_r(&seed)%(9)+1;
        printf("%d ", rand_val);
    }

    printf("\n");

    printf("%.*s", 5, "=================");

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void writeData() {
    FILE *file = fopen("plot.dat", "w");
    if (file == NULL) {
        perror("Failed to open plot.dat");
        exit(1);
    }

    for (int i = 1, j = 400; i <= 400; i++, j--) {
        fprintf(file, "%d\t%d\t%d\n", i, i + 5, j);
        fclose(file); // Close and reopen the file to simulate appending.
        file = fopen("plot.dat", "a");
        if (file == NULL) {
            perror("Failed to open plot.dat");
            exit(1);
        }
        usleep(10000);
    }

    fclose(file);
}

int test2() {
    // Remove the existing plot.dat file, if any.
    if (remove("plot.dat") == 0) {
        printf("plot.dat removed\n");
    }

    FILE *gnuplotPipe = popen("gnuplot", "w");
    if (gnuplotPipe == NULL) {
        perror("Error opening Gnuplot pipe\n");
        return 1;
    }

    // Start a separate process to write data in the background.
    if (fork() == 0) {
        writeData();
        exit(0);
    } else {
        // Run Gnuplot with a script file to update the graph.
        fprintf(gnuplotPipe, "load 'liveplot.gnu'\n");
        fflush(gnuplotPipe);
    }

    // Close the Gnuplot pipe.
    pclose(gnuplotPipe);

    return 0;
}

#define START_TEMPERATURE ((double)(1620 / 2))
#define TEMPERATURE_CEILING 0.00273852

int test3(void) {
    double delta = 0.1;
    double e_p = START_TEMPERATURE;
    double temperature = e_p;
    double start, end, cpu;
    int test = 0;
    start = omp_get_wtime();
    while (temperature >= TEMPERATURE_CEILING)
    {
        for(int i = 0; i < 81; i++) {
            test++;
        }
        temperature = temperature / (1 + (log(1 + delta) / (e_p + 1)) * temperature);
    }
    end = omp_get_wtime();
    cpu = end - start;
    printf("%lf\n", cpu);
    return 0;
}

int main(void) {
    char * cost_file = "COST_PER_DIFFICULTY-46039.txt";
    char * tries_file = "TRIES_PER_DIFFICULTY-46039.txt";
    char * cpu_file = "CPU_USAGE-46039.txt";
    sudoku_plot_performance(MAX_TRIES, cpu_file);
    sudoku_plot_cost_per_difficulty(cost_file);
    sudoku_plot_tries_per_difficulty(tries_file);
    return 0;
}