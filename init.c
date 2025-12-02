#include "game.h"

void init_ncurses() {
    initscr();
    start_color();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    init_pair(WHITE_ON_BLACK_PAIR, COLOR_WHITE, COLOR_BLACK);
    init_pair(RED_ON_BLACK_PAIR, COLOR_RED, COLOR_BLACK);
    init_pair(YELLOW_ON_BLACK_PAIR, COLOR_YELLOW, COLOR_BLACK);
}