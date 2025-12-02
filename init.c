#include "game.h"
#include <ncurses.h>

void init_ncurses() {
    initscr();
    start_color();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    init_pair(WHITE_ON_BLACK_PAIR, COLOR_WHITE, COLOR_BLACK);
    init_pair(RED_ON_BLACK_PAIR, COLOR_RED, COLOR_BLACK);
    init_pair(YELLOW_ON_BLACK_PAIR, COLOR_YELLOW, COLOR_BLACK);
    init_pair(GREEN_ON_BLACK_PAIR, COLOR_GREEN, COLOR_BLACK);
    init_pair(BLUE_ON_BLACK_PAIR, COLOR_BLUE, COLOR_BLACK);
    init_pair(CYAN_ON_BLACK_PAIR, COLOR_CYAN, COLOR_BLACK);
    init_pair(MAGENTA_ON_BLACK_PAIR, COLOR_MAGENTA, COLOR_BLACK);
}