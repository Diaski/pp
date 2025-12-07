#include "game.h"
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
    mvwprintw(win->window, T_MARGIN, L_MARGIN, "Player: %s || Life Force: %-3d", name,hp);
    mvwprintw(win->window, T_MARGIN + 1, L_MARGIN, "Level: %d", level);
    mvwprintw(win->window, T_MARGIN + 2, L_MARGIN, "Time Left: %-4d", time_left/USC_T_MSC);
    mvwprintw(win->window, T_MARGIN + 3, L_MARGIN, "Stars: %d/%-3d", stars, stars_quota);
    wattroff(win->window, COLOR_PAIR(WHITE_ON_BLACK_PAIR));
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
//display congratulations sign with player name and level number everything is centered unless name is too long or level number is too big
//its drawing in parts to create a small animation effect
void draw_sign_congratulations(char* player_name, int level_num,Win* win,int res){
    wattron(win->window, COLOR_PAIR(RED_ON_BLACK_PAIR));
    if(res==0){
        mvwprintw(win->window, T_MARGIN,  (CONGRATULATIONS_WIN_COLS - 25) / 2, "Congratulations! GOOD JOB"); //its 25 because "Congratulations!" has 25 chars and i want to avoid unnecessary calculations
        mvwprintw(win->window, T_MARGIN + 1,  (CONGRATULATIONS_WIN_COLS - (int)strlen(player_name)) / 2, "%s", player_name);
    }
    else if(res==EXIT_GAME){
        mvwprintw(win->window, T_MARGIN,  (CONGRATULATIONS_WIN_COLS - 27) / 2, "Level Incomplete! TRY AGAIN"); 
    }
    else {
        mvwprintw(win->window, T_MARGIN,  (CONGRATULATIONS_WIN_COLS - 11) / 2, "GAME OVER!"); 
    }
    wnoutrefresh(win->window);
    doupdate();
    usleep(CONGRATULATIONS_SLEEP_TIME_USC);
    if(res==0){
        mvwprintw(win->window, T_MARGIN + 2,  (CONGRATULATIONS_WIN_COLS - 27 - (int)log10(level_num)) / 2, "You have completed Level %d!!", level_num);
        wnoutrefresh(win->window);
        doupdate();
    }
    wattroff(win->window, COLOR_PAIR(RED_ON_BLACK_PAIR));
}
//display congrats and wait for some time
void congratulate_player_win(char* player_name, int level_num,int res){
    clear();
    refresh();
    Win* win=create_window(CONGRATULATIONS_WIN_ROWS,CONGRATULATIONS_WIN_COLS,CONGRATULATIONS_WIN_X,CONGRATULATIONS_WIN_Y,false);
    draw_sign_congratulations(player_name, level_num, win,res);
    usleep(CONGRATULATIONS_SLEEP_TIME_USC);
    clear();
    refresh();
    del_window(win);
}
//get player name from input window
char* get_player_name_window(void){
    char* name=(char*)calloc(PLAYER_NAME_MAX,sizeof(char));
    Win* win = create_window(GET_PLAYER_NAME_WIN_ROWS, GET_PLAYER_NAME_WIN_COLS, 0, 0, false);
    mvwprintw(win->window, T_MARGIN, L_MARGIN, "Enter your name (max %d chars): ", PLAYER_NAME_MAX -1);
    mvwprintw(win->window, T_MARGIN + 1, L_MARGIN, " ");
    echo();
    wgetnstr(win->window, name, PLAYER_NAME_MAX -1);
    noecho();
    clear();
    refresh();
    if(strlen(name) == 0){
        strcpy(name, DEFAULT_PLAYER_NAME); //default name if none provided
    }

    del_window(win);
    return name;
}
//display level selection menu and get user choice
//if pressed q return EXIT_GAME
void draw_level_ui(Win* win, int replay) {
    draw_border(win);
    mvwprintw(win->window, T_MARGIN, L_MARGIN, "Select Level:");
    
    // Draw Level List
    for (int i = MIN_LEVEL; i <= MAX_LEVELS && i < LEVEL_WIN_ROWS - 5; i++) {
        mvwprintw(win->window, T_MARGIN + i, L_MARGIN, "%d. Level %d", i, i);
    }

    mvwprintw(win->window, T_MARGIN + MAX_LEVELS + 1, L_MARGIN, "Enter number: ");
    if (replay == REPLAY_ENABLED || replay == REPLAY_LOADED) {
        mvwprintw(win->window, LEVEL_WIN_ROWS - 3, L_MARGIN, "Click '%c' to replay", K_REPLAY);
    }
    mvwprintw(win->window, LEVEL_WIN_ROWS - 2, L_MARGIN, "Press '%c' to quit", K_QUIT);
}
int display_level_selection_menu(int* replay){
    clear();
    refresh();
    Win* win=create_window(LEVEL_WIN_ROWS,LEVEL_WIN_COLS,LEVEL_WIN_X,LEVEL_WIN_Y,false);

    draw_level_ui(win, *replay);

    echo();
    wrefresh(win->window);
    flushinp();
    const char choice_char = (char)wgetch(win->window);
    noecho();
    del_window(win);
    clear();
    refresh();
    if(choice_char == K_QUIT){
        return EXIT_GAME;
    }
    //check for replay
    if(choice_char == K_REPLAY &&  (*replay == REPLAY_LOADED || *replay == REPLAY_ENABLED)){
        *replay = REPLAY_ENABLED;
        return REPLAY_GAME;
    }
    else{
        *replay = REPLAY_DISABLED;
    }
    int choice = choice_char - '0';
    if(choice < MIN_LEVEL || choice > MAX_LEVELS){
        choice = DEFAULT_LEVEL;
    }
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


void change_blink_sprite_base_on_direction(Player_t* p){
    if(p->obj.dx > 0){
        p->obj.current_sprite = p->blink_sprites.right;
    } else if(p->obj.dx < 0){
        p->obj.current_sprite = p->blink_sprites.left;
    } else if(p->obj.dy > 0){
        p->obj.current_sprite = p->blink_sprites.down;
    } else if(p->obj.dy < 0){
        p->obj.current_sprite = p->blink_sprites.up;
    }
}

//draw object to both window and map with specified map character, in order to keep them in sync in case someone forgets
void draw_to_win_and_map(GameObject_t obj, Win* win, char map_char){
    draw_to_map_obj(obj, win, map_char);
    draw_obj(obj, win);
}
//remove object from both window and map in order to keep them in sync in case someone forgets
void remove_from_win_and_map(GameObject_t obj, Win* win){
    remove_from_map_obj(obj, win);
    remove_obj_from_window(obj, win);
}

//display leaderboard with entries and wait for some time
void show_leaderboard(void){
    clear();
    refresh();
    Win* win = create_window(LEADERBOARD_WIN_ROWS, LEADERBOARD_WIN_COLS, LEADERBOARD_WIN_X, LEADERBOARD_WIN_Y, false);
    
    draw_border(win);
    mvwprintw(win->window, T_MARGIN, L_MARGIN, "Leaderboard:");
    const int entry_count = count_leaderboard_entries();
    if (!entry_count) {
        mvwprintw(win->window, T_MARGIN + 1, L_MARGIN, "No leaderboard data available."); //if no entries show this message
    }
    else {
        LeaderboardEntry_t* entries = load_leaderboard_entries(entry_count);
        for (int i = 0; i < entry_count; i++) {
            // 3 is the starting row and -2 to leave space for border
            if (T_MARGIN + 2 + i >= LEADERBOARD_WIN_ROWS - 2) break;
            mvwprintw(win->window, T_MARGIN + 2 + i, L_MARGIN, "%d. %s - %d", i + 1, entries[i].name, entries[i].score);
            
            if (T_MARGIN + 2 + i >= LEADERBOARD_WIN_ROWS - 2) {
                mvwprintw(win->window, LEADERBOARD_WIN_ROWS - 2, L_MARGIN, "...");
                break;
            }
        }
        free_leaderboard_entries(entries, entry_count);
    }
    wnoutrefresh(win->window);
    doupdate();
    usleep(LEADERBOARD_SLEEP_TIME_USC);
    clear();
    refresh();
    del_window(win);
}