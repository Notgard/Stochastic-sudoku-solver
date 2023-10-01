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
    "cyan"
};

void sudoku_graph_stats(char *filename)
{
    FILE *gnuplot = popen("gnuplot", "w");
    if (!gnuplot)
    {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    fprintf(gnuplot, "plot \"%s\" t 'Cost Function' w lp\n", filename);
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
        snprintf(command_buffer, FILE_SIZE + 1, " \"%s\" t 'Cost Function' with %s linewidth %d linecolor \"%s\",", str, LINE_TYPE, LINE_THICKNESS,plot_colors[color]);
        strcat(command, command_buffer);
        color++;
        printf("%s\n", str);
    }
    command[strlen(command) - 1] = '\0';
    printf("%s\n", command);

    va_end(args);
    fprintf(gnuplot, "set title \"%s\"\n", "Evolution de la fonction de cout du recuit simmulé");
    fprintf(gnuplot, "%s\n", command);
    fflush(gnuplot);
    fprintf(stdout, "Click Ctrl+d to quit...\n");
    getchar();

    pclose(gnuplot);
    exit(EXIT_SUCCESS);
}

int main(void)
{
    char *file1 = "./data/0000183b305c-(18-50-22).txt";
    char *file2 = "./data/0000183b305c-(18-53-51).txt";
    char *file3 = "./data/0000183b305c-(19-07-11).txt";
    char *file4 = "./data/0000183b305c-(19-04-19).txt";
    printf("[%s, %s, %s, %s]\n", file1, file2, file3, file4);
    // sudoku_graph_stats(file1);

    sudoku_plot_multiple_stats("%s", file3, file4, NULL);

    return EXIT_SUCCESS;
}