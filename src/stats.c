#include "config.h"

#include "stats.h"

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

/// @brief Plot sudoku solving statistics from given file
/// @param filename given file
void sudoku_plot_stats(char *filename)
{
    FILE *gnuplot = popen("gnuplot", "w");
    if (!gnuplot)
    {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    int color = 0;

    if(GET_OUTPUT) {
        fprintf(gnuplot, "set terminal pngcairo\n");
        fprintf(gnuplot, "set output './output/annealing_plot.png'\n");
    }
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

/// @brief Plot multiple sudoku solving statistics from the given file names
/// @param format all the files to plot as line graph
/// @param  
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

    if(GET_OUTPUT) {
        fprintf(gnuplot, "set terminal pngcairo\n");
        fprintf(gnuplot, "set output './output/annealing_plots.png'\n");
    }

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

/// @brief Plot multiple sudoku solving statistics from given file using the program arguments
/// @param argc number of arguments
/// @param argv arrar of arguments
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

    if(GET_OUTPUT) {
        fprintf(gnuplot, "set terminal pngcairo\n");
        fprintf(gnuplot, "set output './output/annealing_plots.png'\n");
    }

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
    char * file4 = "./data/0000183b305c-05-10-2023-(17-22-31).txt"; //with keep best strategy
    char * file5 = "./data/0000183b305c-05-10-2023-(17-22-53).txt"; //without keep best strategy

    sudoku_plot_multiple_stats("%s", file4, file5, NULL);
    //sudoku_plot_multiple_stats("%s", "./data/0000183b305c-07-10-2023-(15-09-25).txt",NULL);
    //sudoku_plot_multiple_stats("%s", "./data/0000183b305c-07-10-2023-(15-18-15).txt",NULL);

    return EXIT_SUCCESS;
}