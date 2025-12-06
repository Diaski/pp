#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>


int main(void){
    init_ncurses();
    char* player_name = get_player_name_window();
    int res =0;
    while(res ==0 || res == PLAYER_DIED){
        res = level_selector(player_name);
    }
    endwin();   
    free((void*)player_name);
    if (res == LOADING_CONFIG_ERROR){
        printf("Error loading level configuration.\n");
    } else if (res == TERMINAL_SIZE_ERROR){
        printf("Terminal size is too small for the selected level.\n");
    }
}