#include "game.h"

Win* create_window(int rows, int cols, int y, int x) {
    Win* win = malloc(sizeof(Win));
    win->rows = rows;
    win->cols = cols;
    win->y = y;
    win->x = x;
    win->window = newwin(rows, cols, y, x);
    leaveok(win->window,TRUE);
    draw_border(win);
    return win;
}

void draw_border(Win* win) {
    box(win->window, 0, 0);
}

void update_status_display(Win* win, int time, int level) {
    wattron(win->window, COLOR_PAIR(WHITE_ON_BLACK_PAIR));
    mvwprintw(win->window,1,2,"skibidi");
    mvwprintw(win->window, 1, 2, "Level: %d",level);
    mvwprintw(win->window, 2, 2, "| Time: %-5d",time / 1000);
    wattroff(win->window, COLOR_PAIR(WHITE_ON_BLACK_PAIR));
}
void draw_Welcome_Screen(Win* screen){
    draw_border(screen);
    //to be done//
}
void mount_upd(Win* win){
    wnoutrefresh(win->window);
}
void del_window(Win* win){
    delwin(win->window);
    free(win);
}