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

void update_status_display(Win* win, int x, int y,int gamespeed,char input,int hp){
    wattron(win->window, COLOR_PAIR(WHITE_ON_BLACK_PAIR));
    mvwprintw(win->window,1,2,"skibidi");
    mvwprintw(win->window, 2, 3, "Level: %-5d",x);
    mvwprintw(win->window, 3, 4, "| Time: %-5d",y);
    mvwprintw(win->window, 4, 5, "| Speed: %-5d",gamespeed);
    mvwprintw(win->window, 5, 6, "| Input: %-5c",input);
    mvwprintw(win->window, 6, 7, "| HP: %-5d",hp);
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

void remove_obj_from_window(GameObject_t obj, Win* win){
    for(int row = 0; row < obj.height; row++){
        for(int col = 0; col < obj.width; col++){
            mvwaddch(win->window, obj.y + row, obj.x + col, ' ');
        }
    }
}
void select_color(GameObject_t obj,Win* win){
    if(obj.color > 0 && obj.color <= COLOR_PAIR_COUNT){
        wattron(win->window, COLOR_PAIR(obj.color));
    }
    else{
        wattron(win->window, COLOR_PAIR(WHITE_ON_BLACK_PAIR));
    }
}
void deselect_color(GameObject_t obj,Win* win){
    if(obj.color > 0 && obj.color <= COLOR_PAIR_COUNT){
        wattroff(win->window, COLOR_PAIR(obj.color));
    }
    else{
        wattroff(win->window, COLOR_PAIR(WHITE_ON_BLACK_PAIR));
    }
}
void draw_obj(const GameObject_t obj, Win* win){
    select_color(obj,win);
    for(int row = 0; row < obj.height; row++){
        for(int col = 0; col < obj.width; col++){
            mvwaddch(win->window, obj.y + row, obj.x + col, obj.current_sprite[row * obj.width + col]);
        }
    }
    deselect_color(obj,win);
}