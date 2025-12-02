#include "game.h"
void random_enemy_spawn(Win* win, LevelConfig_t* config, Enemy_t** hunters, int* hunters_count){
    if(rand()%config->hunter_spawn_rate==0 && *hunters_count < config->max_enemys_per_level){
        Enemy_t* new_enemy=spawn_enemy(win, config, rand()%(config->hunter_type_count-1)+1);
        if(new_enemy != NULL){
            hunters[*hunters_count]=new_enemy;
            (*hunters_count)++;
        }
    }

}
void move_enemy_list(Enemy_t** enemy, int count, Win* win, Player_t* player){
    for (int i = 0; i < count; i++) {
        if(enemy[i]->alive){
            move_enemy(enemy[i], win, player);
        }
    }
}
int game_loop(Win* main_win, Win* status_win, Player_t* player, int game_speed,LevelConfig_t* config,Enemy_t** hunters,int* hunters_count){
    char input,last_input;
    if (hunters == NULL) {
        return(1);
    }
    while (1)
    {   
        nodelay(stdscr, TRUE);
        last_input = getch();
        if(last_input != ERR){       
            input = last_input;
        }
        if(input == 'q' || player->life_force <= 0){
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
        random_enemy_spawn(main_win, config, hunters, hunters_count);
        move_enemy_list(hunters, *hunters_count, main_win, player);
        update_status_display(status_win, player->obj.x, player->obj.y,game_speed,input,player->life_force);
        draw_obj(player->obj, main_win);
        wnoutrefresh(main_win->window);
        mount_upd(status_win);
        doupdate();
        input=0;
        usleep(game_speed * 1000);
    }
    return 0;
}
int level_selector(int level_num){
    LevelConfig_t* config = load_level_config(level_num);
    Enemy_t** hunters=malloc(sizeof(Enemy_t*) * config->max_enemys_per_level);
    int hunters_count=0;
    if (config == NULL) {
        printf("Failed to load level configuration.\n");
        return 1;
    }
    if(config->seed == 0){
        exit(1);
    } else {
        srand(config->seed);
    }

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
    if(!game_loop(main_win,status_win,player, game_speed,config,hunters,&hunters_count)){
        //endgame condition
    }
    destroy_player(player);
    destroy_enemy_list(hunters, hunters_count);
    delwin(main_win->window);
    delwin(status_win->window);
    endwin();
    free_map(main_win->map, main_win->rows);
    free(main_win);
    free(status_win);
    free_level_config(config);
    return 0;
}
int main(){
    init_ncurses();
    level_selector(2);
    return 0;
}