#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "config.h"

const char plot_colors[10][10] = {
    "black",
    "red",
    "green",
    "blue",
    "yellow",
    "purple",
    "cyan",
    "pink",
    "orange",
    "violet"
};

/// @brief Dessine une courbe et un histogramme montrant le temps d'execution en moyenne de chaque niveau de difficulté de sudoku 
/// @param nb_tries 
/// @param filename 
void sudoku_plot_performance(int nb_tries, const char *filename)
{
    char command_buffer[COMMANDE_SIZE + 1];
    char command[COMMANDE_SIZE + 1] = "plot";

    FILE *gnuplot = popen("gnuplot", "w");
    if (!gnuplot)
    {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    snprintf(command_buffer,
             FILE_SIZE + 1,
             " \"%s\" using 2:xtic(1) t 'Moyenne du temps execution' with %s linewidth %d linecolor \"%s\", \"\" \
             using 2:xtic(1) with histogram title 'Performance' fillstyle pattern 4 linecolor \"green\"",
             filename, "linespoints", LINE_THICKNESS, "black");
    strcat(command, command_buffer);
    printf("%s\n", command);

    fprintf(gnuplot, "set title \"Temps d'execution en moyenne de chaque type de sudoku (%d essais)\" font \"%s\"\n", nb_tries, "Helvetica,18");
    fprintf(gnuplot, "set xlabel \"Difficulté du sudoku\"\n");
    fprintf(gnuplot, "set ylabel \"Temps d'execution (secondes)\"\n");
    fprintf(gnuplot, "%s\n", command);
    fflush(gnuplot);
    fprintf(stdout, "Click Ctrl+d to quit...\n");
    getchar();

    pclose(gnuplot);
    exit(EXIT_SUCCESS);
}

void sudoku_plot_difficulty_benchmark(int argc, char *argv[], const char *difficulty)
{
    int color = 0;
    //double m = atof(argv[3]);
    //double b = atof(argv[4]);
    char command_buffer[COMMANDE_SIZE + 1];
    char command[COMMANDE_SIZE + 1] = "plot";
    char * filename = argv[2];

    FILE *gnuplot = popen("gnuplot", "w");
    if (!gnuplot)
    {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    snprintf(command_buffer,
             FILE_SIZE + 1,
             " \"%s\" using 1:2 with %s lc rgb \"%s\" title '%s',",// '%s' using 1:($1 * %f + %f) smooth unique with lines title \"%s\"
             filename, "points pointtype 7 pointsize 2", plot_colors[color], "Données (execution/cout algo)"/* , 
             filename, m, b, "regression linéaire" */);

    strcat(command, command_buffer);
    printf("%s\n", filename);

    command[strlen(command) - 1] = '\0';
    printf("%s\n", command);

    fprintf(gnuplot, "set title \"Résultats des tests de l\'algorithme pour des sudoku %s\" font \"%s\"\n", difficulty, "Helvetica,18");
    fprintf(gnuplot, "set xlabel \"Temps d'execution du test (en secondes)\"\n");
    fprintf(gnuplot, "set ylabel \"Meilleur coût obtenu\"\n");
    fprintf(gnuplot, "set grid\n");
    fprintf(gnuplot, "f(x) = m*x + b\n");
    fprintf(gnuplot, "fit f(x) '%s' using 1:2 via m,b\n", filename);
    fprintf(gnuplot, "%s, f(x) title 'Regression linéaire'\n", command);
    fflush(gnuplot);
    fprintf(stdout, "Click Ctrl+d to quit...\n");
    getchar();

    pclose(gnuplot);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    int mode = atoi(argv[1]);
    switch (mode)
    {
    case 1: // easy sudoku
        sudoku_plot_difficulty_benchmark(argc, argv, "easy");
        break;
    case 2: // medium sudoku
        sudoku_plot_difficulty_benchmark(argc, argv, "medium");
        break;
    case 3: // hard sudoku
        sudoku_plot_difficulty_benchmark(argc, argv, "hard");
        break;
    case 4: //all of them
        sudoku_plot_performance(MAX_TRIES, argv[2]);
        break;
    }

    return EXIT_SUCCESS;
}