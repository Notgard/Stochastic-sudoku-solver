#ifndef _COLORS_
#define _COLORS_

// Colors constants
#define WHITE       1
#define GREEN       2
#define BLUE        3
#define RED         4 
#define YELLOW      5
#define CYAN        6
#define MAGENTA     7
#define FD_WHITE    8
#define FD_GREEN    9
#define FD_BLUE    10
#define FD_RED     11
#define FD_YELLOW  12
#define FD_CYAN    13
#define FD_MAGENTA 14


/**
 * Palette definition.
 */
void palette();

/**
 * @brief Translates the background color to the foreground color
 * 
 * @param int color the background color
 * @return int the foreground color
 */
int bgtofg(int bgcolor);

/**
 * @brief Translates the foreground color to the background color
 * 
 * @param int color the foreground color
 * @return int the background color
 */
int fgtobg(int fgcolor);

#endif