#include "config.h"

#include "benchmark.h"

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

/// @brief Creates histogram of the average execution time of the sudoku solving algorithm for each sudoku difficulty using the given file
/// @param nb_tries the number of tries of the algorithm
/// @param filename the given file
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

    if(GET_OUTPUT) {
        fprintf(gnuplot, "set terminal pngcairo\n");
        fprintf(gnuplot, "set output './output/histo_plot_bench.png'\n");
    }

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

/// @brief Creates point plot with linear regression curve of the execution time of the tests performed of the given difficulty level
/// @param argc the number of arguments
/// @param argv the list of files 
/// @param difficulty the difficulty of the tested
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

    if(GET_OUTPUT) {
        fprintf(gnuplot, "set terminal pngcairo\n");
        fprintf(gnuplot, "set output './output/%s_bench.png'\n", difficulty);
    }

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

/// @brief Creates box plot of the cost distribution of each sudoku difficulty from the given file
/// @param filename the given file
void sudoku_plot_cost_per_difficulty(const char *filename)
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
             " for [i=1:4] '%s' using (i):i title '%s #'.i",
             filename, "Distribution du coût");
    strcat(command, command_buffer);
    printf("%s\n", command);

    if(GET_OUTPUT) {
        fprintf(gnuplot, "set terminal pngcairo\n");
        fprintf(gnuplot, "set output './output/cost_box_plot.png'\n");
    }

    fprintf(gnuplot, "set title \"Distribution des meilleurs coût de l'algorithm pour chaque type de sudoku (%d essais)\" font \"%s\"\n", MAX_TRIES, "Helvetica,16");
    fprintf(gnuplot, "set xlabel \"Difficulté du sudoku\"\n");
    fprintf(gnuplot, "set ylabel \"Meilleur coût retourné par l'algorithme\"\n");
    fprintf(gnuplot, "set style fill solid 0.25 border -1\n");
    fprintf(gnuplot, "set style boxplot outliers pointtype 7\n");
    fprintf(gnuplot, "set style data boxplot\n");
    fprintf(gnuplot, "set xtics ('easy' 1, 'medium' 2, 'hard' 3, 'diabolical' 4)\n");

    fprintf(gnuplot, "%s\n", command);
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
    case 4: //benchmark all of them
        sudoku_plot_performance(MAX_TRIES, argv[2]);
        break;
    case 5: //stats for all of them
        sudoku_plot_cost_per_difficulty(argv[2]);
        break;
    }

    return EXIT_SUCCESS;
}