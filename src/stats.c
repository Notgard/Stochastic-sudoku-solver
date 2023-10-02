#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "config.h"

const char plot_colors[6][10] = {
    "red",
    "green",
    "blue",
    "yellow",
    "purple",
    "cyan"};

void sudoku_graph_stats(char *filename)
{
    FILE *gnuplot = popen("gnuplot", "w");
    if (!gnuplot)
    {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    int color = 0;

    fprintf(gnuplot, "set title \"%s\" font \"%s\"\n", "Evolution de la fonction de cout du recuit simmulé", "Helvetica,18");
    fprintf(gnuplot, "set xlabel \"Redémarrage de l'algorithme\"\n");
    fprintf(gnuplot, "set ylabel \"Valeur de la fonction de cout\"\n");
    fprintf(gnuplot, "set xtics 0, %d\n", X_RANGE);

    fprintf(gnuplot, "plot \"%s\" t 'Cost Function' with %s linewidth %d linecolor \"%s\"\n", 
    filename, LINE_TYPE, LINE_THICKNESS, plot_colors[color]);

    fflush(gnuplot);
    fprintf(stdout, "Click Ctrl+d to quit...\n");
    getchar();

    pclose(gnuplot);
    exit(EXIT_SUCCESS);
}

void sudoku_plot_multiple_stats(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    int color = 0;
    char command_buffer[COMMANDE_SIZE + 1];
    char command[COMMANDE_SIZE + 1] = "plot";

    FILE *gnuplot = popen("gnuplot", "w");
    if (!gnuplot)
    {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        const char *str = va_arg(args, const char *);
        if (str == NULL)
        {
            break;
        }
        snprintf(command_buffer,
                 FILE_SIZE + 1,
                 " \"%s\" t 'Cost Function' with %s linewidth %d linecolor \"%s\",",
                 str, LINE_TYPE, LINE_THICKNESS, plot_colors[color]);

        strcat(command, command_buffer);
        color++;
        printf("%s\n", str);
    }
    command[strlen(command) - 1] = '\0';
    printf("%s\n", command);

    va_end(args);
    fprintf(gnuplot, "set title \"%s\" font \"%s\"\n", "Evolution de la fonction de cout du recuit simmulé", "Helvetica,18");
    fprintf(gnuplot, "set xlabel \"Redémarrage de l'algorithme\"\n");
    fprintf(gnuplot, "set ylabel \"Valeur de la fonction de cout\"\n");
    fprintf(gnuplot, "set xtics 0, %d\n", X_RANGE);
    // fprintf(gnuplot, "set ytics 0, %d\n", Y_RANGE);
    fprintf(gnuplot, "%s\n", command);
    fflush(gnuplot);
    fprintf(stdout, "Click Ctrl+d to quit...\n");
    getchar();

    pclose(gnuplot);
    exit(EXIT_SUCCESS);
}

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

    fprintf(gnuplot, "set title \"%s\" font \"%s\"\n", "Temps d'execution en moyenne de chaque type de sudoku", "Helvetica,18");
    fprintf(gnuplot, "set xlabel \"Temps d'execution (secondes)\"\n");
    fprintf(gnuplot, "set ylabel \"Difficulté du sudoku\"\n");
    fprintf(gnuplot, "%s\n", command);
    fflush(gnuplot);
    fprintf(stdout, "Click Ctrl+d to quit...\n");
    getchar();

    pclose(gnuplot);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    char *file1 = "./data/00005f662e09-(20-30-35).txt";
    char *file2 = "./data/00005f662e09-(20-30-36).txt";
    char *file3 = "./data/00005f662e09-(20-30-37).txt";
    char *file4 = "./data/00005f662e09-(20-30-38).txt";
    char * file5 = "./data/0000183b305c-02-10-2023-(13-36-44).txt";
    char * file6 = "./data/0000183b305c-02-10-2023-(13-51-11).txt";
    char * file7 = "./data/0000183b305c-02-10-2023-(13-53-53).txt";
    printf("[%s, %s, %s, %s, %s, %s, %s]\n", file1, file2, file3, file4, file5, file6, file7);

    sudoku_graph_stats(file5);

    // sudoku_plot_multiple_stats("%s", file5, file6, file7, NULL);

    //const char *file8 = "./benchmark/2866.txt";
    //sudoku_plot_performance(MAX_TRIES, file8);

    return EXIT_SUCCESS;
}