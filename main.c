#include "game.h"

/*int main(){
    init_ncurses();
    Win* main_win;
    Win* status_win;
    create_window(main_win, 20, 40, 0, 0);
    create_window(status_win, 20, 40, 20, 0);
    
    refresh();

    draw_border(main_win);
    draw_border(status_win);
    wnoutrefresh(main_win->window);
    wnoutrefresh(status_win->window);
    doupdate();
    while(getch()!='q'){
        
    }

    delwin(main_win->window);
    delwin(status_win->window);
    endwin();
    return 0;
}*/
int game_loop(Win* main_win, Win* status_win, Player_t* player){
    char input,last_input;
    while (1)
    {
        nodelay(stdscr, TRUE);
        input = getch();
        if(input != ERR){       
            last_input = input;
        } else {
            input = last_input;
        }
        while (getch() != ERR);
        nodelay(stdscr, FALSE);
        if(input == 'q'){
            return 0;
        }
        remove_player_from_window(player, main_win);
        handle_player_input(player, input,main_win);
        update_status_display(status_win, player->obj.x, player->obj.y);
        draw_player(player, main_win);
        wnoutrefresh(main_win->window);
        mount_upd(status_win);
        doupdate();
        usleep(100000);
    }
    
    return 0;
}
int main(){
    LevelConfig_t* config = load_level_config(1);
    init_ncurses();
    Win* main_win=create_window(config->map_rows,config->map_cols,0,0,1);
    Win* status_win=create_window(config->status_rows,config->status_cols,0,config->map_rows,0);
    Player_t* player=create_player(config);
    refresh();
    mount_upd(main_win);
    mount_upd(status_win);
    draw_player(player,main_win);
    wnoutrefresh(main_win->window);
    doupdate();
    if(!game_loop(main_win,status_win,player)){
        //endgame condition
    }
    destroy_player(player);
    delwin(main_win->window);
    delwin(status_win->window);
    endwin();
    free_map(main_win->map, main_win->rows);
    free(main_win);
    free(status_win);
    free_level_config(config);
}