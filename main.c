#include "game.h"

int game_loop(Win* main_win, Win* status_win, Player_t* player, int game_speed,LevelConfig_t* config){
    char input,last_input;
    Enemy_t* hunter=spawn_enemy(main_win, config, 2);
    while (1)
    {   
        nodelay(stdscr, TRUE);
        last_input = getch();
        if(last_input != ERR){       
            input = last_input;
        }
        if(input == 'q'){
            if(hunter !=NULL){
                destroy_enemy(hunter);
            }
            return 0;
        }
        if(input == 'p' && game_speed -config->delta_speed >= config->max_speed){
            game_speed -= config->delta_speed;
        }
        if(input == 'o' && game_speed +config->delta_speed <= config->min_speed){
            game_speed += config->delta_speed;
        }
        remove_obj_from_window(player->obj, main_win);
        handle_player_input(player, input,main_win);
        if(hunter->alive == 1){
            move_enemy(hunter, main_win, player);
        }
        update_status_display(status_win, player->obj.x, player->obj.y,game_speed,input);
        draw_obj(player->obj, main_win);
        wnoutrefresh(main_win->window);
        mount_upd(status_win);
        doupdate();
        input=0;
        usleep(game_speed * 1000);
    }
    if(hunter !=NULL){
        destroy_enemy(hunter);
    }
    return 0;
}
int main(){
    LevelConfig_t* config = load_level_config(1);
    if (config == NULL) {
        printf("Failed to load level configuration.\n");
        return 1;
    }
    if(config->seed == 0){
        exit(1);
    } else {
        srand(config->seed);
    }
    init_ncurses();
    Win* main_win=create_window(config->map_rows,config->map_cols,0,0,1);
    Win* status_win=create_window(config->status_rows,config->status_cols,0,config->map_rows,0);
    Player_t* player=create_player(config);
    int game_speed = config->min_speed;
    refresh();
    mount_upd(main_win);
    mount_upd(status_win);
    draw_obj(player->obj,main_win);
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