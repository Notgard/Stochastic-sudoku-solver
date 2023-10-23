#include <stdio.h>
#include <stdlib.h>

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

int main(void) {
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

    return 0;
}