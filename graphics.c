#include "game.h"


Win* create_window(int rows,int cols,int x, int y,int have_map) {
    Win* win = malloc(sizeof(Win));
    win->rows = rows;
    win->cols = cols;
    win->y = y;
    win->x = x;
    win->window = newwin(win->rows, win->cols, win->y, win->x);
    if(have_map) {
        create_map(win);
    }
    leaveok(win->window,TRUE);
    draw_border(win);
    return win;
}

void draw_border(Win* win) {
    wattron(win->window, COLOR_PAIR(WHITE_ON_BLACK_PAIR));  
    box(win->window, 0, 0);
    wattroff(win->window, COLOR_PAIR(WHITE_ON_BLACK_PAIR));
}

void update_status_display(Win* win, int x, int y) {
    wattron(win->window, COLOR_PAIR(WHITE_ON_BLACK_PAIR));
    mvwprintw(win->window,1,2,"skibidi");
    mvwprintw(win->window, 2, 3, "Level: %-5d",x);
    mvwprintw(win->window, 3, 4, "| Time: %-5d",y);
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

void remove_player_from_window(Player_t* player, Win* win){
    for(int row = 0; row < player->obj.height; row++){
        for(int col = 0; col < player->obj.width; col++){
            mvwaddch(win->window, player->obj.y + row, player->obj.x + col, ' ');
        }
    }
}

void draw_player(const Player_t* p,Win* win){
    for(int row = 0; row < p->obj.height; row++){
        for(int col = 0; col < p->obj.width; col++){
            mvwaddch(win->window, p->obj.y + row, p->obj.x + col, p->obj.current_sprite[row * p->obj.width + col]);
        }
    }
}