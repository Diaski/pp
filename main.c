#include "game.h"

int main(){
    init_ncurses();
    char* player_name = player_name_window();
    const int level_num = display_level_selection_menu();
    printf("Welcome, %s!\n", player_name);
    level_selector(level_num, player_name);
    free(player_name);
}