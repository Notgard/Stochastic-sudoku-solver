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
                 COMMANDE_SIZE + 1,
                 " \"%s\" t 'Cost function file#%d' with %s linewidth %d linecolor \"%s\",",
                 str, color + 1, LINE_TYPE, LINE_THICKNESS, plot_colors[color]);

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
    //fprintf(gnuplot, "set xtics 0, %d\n", X_RANGE);
    // fprintf(gnuplot, "set ytics 0, %d\n", Y_RANGE);
    fprintf(gnuplot, "%s\n", command);
    fflush(gnuplot);
    fprintf(stdout, "Click Ctrl+d to quit...\n");
    getchar();

    pclose(gnuplot);
    exit(EXIT_SUCCESS);
}

void sudoku_plot_statistics(int argc, char *argv[])
{
    int color = 0;
    char command_buffer[COMMANDE_SIZE + 1];
    char command[COMMANDE_SIZE + 1] = "plot";

    FILE *gnuplot = popen("gnuplot", "w");
    if (!gnuplot)
    {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i++)
    {
        snprintf(command_buffer,
                 FILE_SIZE + 1,
                 " \"%s\" t 'Cost function' with %s linewidth %d linecolor \"%s\",",
                 argv[i], LINE_TYPE, LINE_THICKNESS, plot_colors[color]);

        strcat(command, command_buffer);
        color++;
        printf("%s\n", argv[i]);
    }
    command[strlen(command) - 1] = '\0';
    printf("%s\n", command);

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

int main(int argc, char *argv[])
{
    char *file1 = "data/0004bab224ce-03-10-2023-(21-56-46).txt";
    char *file2 = "data/0004686971dc-03-10-2023-(21-56-34).txt";
    char *file3 = "data/0004464633b4-03-10-2023-(21-56-24).txt";
    char *file4 = "data/0004125f628d-03-10-2023-(21-56-13).txt";
    char *file5 = "data/0003af4e2943-03-10-2023-(21-56-03).txt";
    char *file6 = "data/00031006ebf1-03-10-2023-(21-55-53).txt";
    char *file7 = "data/00023580f347-03-10-2023-(21-55-43).txt";
    char *file8 = "data/000212406270-03-10-2023-(21-55-33).txt";
    char *file9 = "data/0001d5d6314e-03-10-2023-(21-55-23).txt";
    char *file10 = "data/0000183b305c-03-10-2023-(21-55-13).txt";

    printf("[%s, %s, %s, %s, %s, %s, %s]\n", file1, file2, file3, file4, file5, file6, file7);

    // sudoku_graph_stats(file5);
    sudoku_plot_multiple_stats("%s", file1, file2,
                               file3,
                               file4,
                               file5,
                               file6,
                               file7,
                               file8,
                               file9,
                               file10, NULL);

    return EXIT_SUCCESS;
}