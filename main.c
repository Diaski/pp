#include "game.h"

int game_loop(Win* main_win, Win* status_win, Player_t* player, int game_speed,LevelConfig_t* config){
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
        if(input == 'q'){
            return 0;
        }
        if(last_input == 'p' && game_speed -config->delta_speed >= config->max_speed){
            game_speed -= config->delta_speed;
        }
        if(last_input == 'o' && game_speed +config->delta_speed <= config->min_speed){
            game_speed += config->delta_speed;
        }
        remove_player_from_window(player, main_win);
        handle_player_input(player, last_input,main_win);
        update_status_display(status_win, player->obj.x, player->obj.y,game_speed,input);
        draw_player(player, main_win);
        wnoutrefresh(main_win->window);
        mount_upd(status_win);
        doupdate();
        last_input = 0;
        usleep(game_speed * 1000);
    }
    
    return 0;
}
int main(){
    LevelConfig_t* config = load_level_config(1);
    init_ncurses();
    Win* main_win=create_window(config->map_rows,config->map_cols,0,0,1);
    Win* status_win=create_window(config->status_rows,config->status_cols,0,config->map_rows,0);
    Player_t* player=create_player(config);
    int game_speed = config->min_speed;
    refresh();
    mount_upd(main_win);
    mount_upd(status_win);
    draw_player(player,main_win);
    wnoutrefresh(main_win->window);
    doupdate();
    if(!game_loop(main_win,status_win,player, game_speed,config)){
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