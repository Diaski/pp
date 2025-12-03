#include "game.h"
#include <unistd.h>


Win* create_window(int rows,int cols,int x, int y,int have_map) {
    Win* win = (Win*)malloc(sizeof(Win));
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
    mvwprintw(win->window, 1, 2, "Player: %s", name);
    mvwprintw(win->window, 2, 2, "Level: %d", level);
    mvwprintw(win->window, 3, 2, "Time Left: %-4d", time_left/1000);
    mvwprintw(win->window, 4, 2, "Stars: %d/%-2d", stars, stars_quota);
    mvwprintw(win->window, 5, 2, "Life Force: %-4d", hp);
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
void draw_sign_congratulations(char* player_name, int level_num,Win* win){
    mvwprintw(win->window, 2, 2, "Congratulations, %s!", player_name);
    wnoutrefresh(win->window);
    doupdate();
    usleep(1000000);
    mvwprintw(win->window, 4, 2, "You have completed Level %d!", level_num);
    wnoutrefresh(win->window);
    doupdate();
}
void congratulate_player_win(char* player_name, int level_num){
    Win* win=create_window(20,50,5,5,0);
    draw_border(win);
    draw_sign_congratulations(player_name, level_num, win);
    usleep(1000000);
    clear();
    refresh();
    del_window(win);
}
char* player_name_window(){
    char* name=(char*)malloc(sizeof(char)*PLAYER_NAME_MAX);
    mvprintw(1, 2, "Enter your name (max %d chars): ", PLAYER_NAME_MAX -1);
    echo();
    getnstr(name, PLAYER_NAME_MAX -1);
    noecho();
    clear();
    refresh();
    if(strlen(name) == 0){
        strcpy(name, "Player1");
    }
    return name;
}

int display_level_selection_menu(){
    refresh();
    clear();
    Win* win=create_window(20,40,5,5,0);
    draw_border(win);
    mvwprintw(win->window,2,2,"Select Level:");
    mvwprintw(win->window,4,4,"1. Level 1");
    mvwprintw(win->window,5,4,"2. Level 2");
    mvwprintw(win->window,6,4,"3. Level 3");
    mvwprintw(win->window,7,4,"4. Level 4");
    mvwprintw(win->window,8,4,"5. Level 5");
    mvwprintw(win->window,9,2,"Enter number: ");
    echo();
    wrefresh(win->window);
    const char choice_char = wgetch(win->window);
    noecho();
    delwin(win->window);
    int choice = choice_char - '0';
    if(choice < 1 || choice > 5){
        choice = 1;
    }
    clear();
    free(win);
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