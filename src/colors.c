#include "colors.h"

#include <ncurses.h>

/**
 * Palette definition.
 */
void palette() {
    init_pair(WHITE, COLOR_WHITE, COLOR_BLACK);
    init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(RED, COLOR_RED, COLOR_BLACK);
    init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(FD_WHITE, COLOR_BLACK, COLOR_WHITE);
    init_pair(FD_GREEN, COLOR_BLACK, COLOR_GREEN);
    init_pair(FD_BLUE, COLOR_BLACK, COLOR_BLUE);
    init_pair(FD_RED, COLOR_BLACK, COLOR_RED);
    init_pair(FD_YELLOW, COLOR_BLACK, COLOR_YELLOW);
    init_pair(FD_CYAN, COLOR_BLACK, COLOR_CYAN);
    init_pair(FD_MAGENTA, COLOR_BLACK, COLOR_MAGENTA);
}

/**
 * @brief Translates the background color to the foreground color
 * 
 * @param int color the background color
 * @return int the foreground color, -1 if given invalid bgcolor
 */
int bgtofg(int bgcolor) {
    if(bgcolor > 7)
        return bgcolor-7;
    return -1;
}

/**
 * @brief Translates the foreground color to the background color
 * 
 * @param int color the foreground color
 * @return int the background color, -1 if given invalid fgcolor
 */
int fgtobg(int fgcolor) {
    if(fgcolor < 8) 
        return fgcolor+7;
    return -1;
}