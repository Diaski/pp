#include "game.h"
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


Win* create_window(int rows,int cols,int x, int y,int have_map) {
    Win* win = (Win*)calloc(1,sizeof(Win));
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

void update_status_display(Win* win, char* name,int level,int time_left,int stars,int stars_quota,int hp){
    wattron(win->window, COLOR_PAIR(WHITE_ON_BLACK_PAIR));
    mvwprintw(win->window, 1, 2, "Player: %s || Life Force: %-4d", name,hp);
    mvwprintw(win->window, 2, 2, "Level: %d", level);
    mvwprintw(win->window, 3, 2, "Time Left: %-4d", time_left/USC_T_MSC);
    mvwprintw(win->window, 4, 2, "Stars: %d/%-2d", stars, stars_quota);
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
    free((void*)win);
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
void draw_sign_congratulations(char* player_name, int level_num,Win* win){
    mvwprintw(win->window, 2, 2, "Congratulations, %s!", player_name);
    wnoutrefresh(win->window);
    doupdate();
    usleep(CONGRATULATIONS_SLEEP_TIME_USC);
    mvwprintw(win->window, 4, 2, "You have completed Level %d!", level_num);
    wnoutrefresh(win->window);
    doupdate();
}
void congratulate_player_win(char* player_name, int level_num){
    clear();
    refresh();
    Win* win=create_window(CONGRATULATIONS_WIN_ROWS,CONGRATULATIONS_WIN_COLS,CONGRATULATIONS_WIN_X,CONGRATULATIONS_WIN_Y,0);
    draw_border(win);
    draw_sign_congratulations(player_name, level_num, win);
    usleep(CONGRATULATIONS_SLEEP_TIME_USC);
    clear();
    refresh();
    del_window(win);
}
char* player_name_window(void){
    char* name=(char*)calloc(PLAYER_NAME_MAX,sizeof(char));
    Win* win = create_window(5, 40, 1, 1, 0);
    mvwprintw(win->window, 1, 2, "Enter your name (max %d chars): ", PLAYER_NAME_MAX -1);
    echo();
    wgetnstr(win->window, name, PLAYER_NAME_MAX -1);
    noecho();
    clear();
    refresh();
    if(strlen(name) == 0){
        strcpy(name, "Player1");
    }

    del_window(win);
    return name;
}

int display_level_selection_menu(void){
    clear();
    refresh();
    int counter =2;
    Win* win=create_window(LEVEL_WIN_ROWS,LEVEL_WIN_COLS,LEVEL_WIN_X,LEVEL_WIN_Y,0);
    draw_border(win);
    mvwprintw(win->window,2,2,"Select Level:");
    for (int i=0;i<MAX_LEVELS+1;i++){
        if(i>=LEVEL_WIN_ROWS-4) break;
        mvwprintw(win->window,4+i,4,"1. Level %c", '1'+i);
        counter++;
    }
    mvwprintw(win->window,counter+1,2,"Enter number: ");
    mvwprintw(win->window,LEVEL_WIN_ROWS-2,1,"press 'q' to quit");
    echo();
    wrefresh(win->window);
    flushinp();
    const char choice_char = (char)wgetch(win->window);
    noecho();
    delwin(win->window);
    if(choice_char == 'q'){
        clear();
        refresh();
        free((void*)win);
        return -1;
    }
    int choice = choice_char - '0';
    if(choice < 1 || choice > MAX_LEVELS){
        choice = 1;
    }
    clear();
    free((void*)win);
    return choice;
}
void change_sprite_base_on_direction(GameObject_t* obj){
    if(obj->dx > 0){
        obj->current_sprite = obj->sprites_list.right;
    } else if(obj->dx < 0){
        obj->current_sprite = obj->sprites_list.left;
    } else if(obj->dy > 0){
        obj->current_sprite = obj->sprites_list.down;
    } else if(obj->dy < 0){
        obj->current_sprite = obj->sprites_list.up;
    }
}
void draw_to_win_and_map(GameObject_t obj, Win* win, char map_char){
    draw_to_map_obj(obj, win, map_char);
    draw_obj(obj, win);
}
void remove_from_win_and_map(GameObject_t obj, Win* win){
    remove_from_map_obj(obj, win);
    remove_obj_from_window(obj, win);
}