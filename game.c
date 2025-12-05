#include "game.h"

void random_enemy_spawn(Win* win, LevelConfig_t* config, Enemy_t** hunters, int* hunters_count,int time_left){
    if(rand()%config->hunter_spawn_rate==0 && *hunters_count < config->max_enemys_per_level){
        Enemy_t* new_enemy=spawn_enemy(win, config, rand()%(config->hunter_type_count), time_left);
        if(new_enemy != NULL){
            hunters[*hunters_count]=new_enemy;
            (*hunters_count)++;
        }
    }

}
void random_star_spawn(Win* win, LevelConfig_t* config, Star_t** stars, int* stars_count){
    if(rand()%config->star_spawn_chance==0 && *stars_count < MAX_STARS){
        Star_t* new_star=spawn_star(win);
        if(new_star != NULL){
            stars[*stars_count]=new_star;
            (*stars_count)++;
        }
    }

}
void move_star_list(Star_t** stars, int count, Win* win, Player_t* player){
    for (int i = 0; i < count; i++) {
        if(stars[i]->alive){
            move_star(stars[i], win, player);
        }
    }
}
void move_enemy_list(Enemy_t*** enemy, int count, Win* win, Player_t* player){
    for (int i = 0; i < count; i++) {
        if((*enemy)[i]->alive){
            move_enemy((*enemy)[i], win, player);
        }
    }
}
void free_all_from_level(LevelConfig_t* config, Enemy_t** hunters, int hunters_count, Star_t** stars, int stars_count, Win* main_win, Win* status_win, Player_t* player){
    destroy_player(player);
    destroy_enemy_list(hunters, hunters_count);
    free_star_list(stars, stars_count);
    delwin(main_win->window);
    delwin(status_win->window);
    endwin();
    free_map(main_win->map, main_win->rows);
    free(main_win);
    free(status_win);
    free_level_config(config);
}
int validate_input(char input, Player_t* player, int* game_speed, LevelConfig_t* config, int time_left){
    if(input == 'q' || player->life_force <= 0 || time_left <= 0){
        return 1;
    }
    if(input == 'p' && *game_speed -config->delta_speed >= config->max_speed){
        *game_speed -= config->delta_speed;
    }
    if(input == 'o' && *game_speed +config->delta_speed <= config->min_speed){
        *game_speed += config->delta_speed;
    }
    return 0;
}
int game_loop(Win* main_win, Win* status_win, Player_t* player, int game_speed,LevelConfig_t* config,Enemy_t** hunters,int* hunters_count,int level_num, Star_t** stars,int* stars_count){
    char input,last_input;
    if (hunters == NULL) {
        return(2);
    }
    nodelay(stdscr, TRUE);
    int time_left = config->time_limit_ms;
    while (1)
    {   
        last_input = getch();
        if(last_input != ERR){       
            input = last_input;
        }
        flushinp();
        if(validate_input(input, player, &game_speed, config, time_left)){
            return PLAYER_DIED;
        }

        handle_player_input(player, input,main_win,config, game_speed);

        random_star_spawn(main_win, config, stars, stars_count);
        move_star_list(stars, *stars_count, main_win, player);

        random_enemy_spawn(main_win, config, hunters, hunters_count, time_left);
        move_enemy_list(&hunters, *hunters_count, main_win, player);

        update_status_display(status_win, player->name,level_num,time_left,player->stars,config->star_quota,player->life_force);
        wnoutrefresh(main_win->window);
        mount_upd(status_win);
        doupdate();

        input=0;
        usleep(game_speed * 1000);
        time_left -= game_speed;
        if(player->stars >= config->star_quota){
            return 0;
        }
    }
    return 1;
}

int check_config(LevelConfig_t* config){
    int res=0;
    if (config == NULL) {
        res = 2;
    }
    if(config->seed == 0){
        res = 2;
    }
    if(config->max_enemys_per_level <0 ){
        res = 2;
    }
    if(LINES < config->map_rows + config->status_rows || COLS < config->map_cols){
        res = 2;
    }
    if(config->player == NULL){
        res = 2;
    }
    if(config->hunters == NULL){
        res = 2;
    }
    if(res !=0){
        free_level_config(config);
        clear();
        refresh();
    }
    return res;
}

void end_game(char* player_name, int level_num, int res){
    if(res == 0){
        congratulate_player_win(player_name, level_num);
    } else {
        printf("Level failed or exited.\n");
    }
}
Win* s_win_create(LevelConfig_t* config,int x,int y,int have_map){ 
    return create_window(config->map_rows,config->map_cols,x,y,have_map); //function to Make level selector cleaner for not writitng so many lines
}
int level_selector(char* player_name){
    const int level_num = display_level_selection_menu();
    if (level_num == -1){
        return 4;
    }

    LevelConfig_t* config = load_level_config(level_num);
    int res = check_config(config);
    if (res != 0){
        return res;
    }

    Enemy_t** hunters=(Enemy_t**)malloc(sizeof(Enemy_t*) * config->max_enemys_per_level);
    Star_t** stars=(Star_t**)malloc(sizeof(Star_t*) * config->max_enemys_per_level);
    Win* main_win=create_window(config->map_rows,config->map_cols,0,0,1);
    Win* status_win=create_window(config->status_rows,config->status_cols,0,config->map_rows,0);
    Player_t* player=create_player(config);
    int stars_count=0;
    int hunters_count=0;
    const int game_speed = config->min_speed;
    strncpy(player->name, player_name, PLAYER_NAME_MAX - 1);
    
    refresh();

    mount_upd(main_win);
    mount_upd(status_win);
    wnoutrefresh(main_win->window);
    doupdate();

    if(res==0) res = game_loop(main_win,status_win,player, game_speed,config,hunters,&hunters_count, level_num, stars, &stars_count);
    
    end_game(player_name, level_num, res);
    
    free_all_from_level(config, hunters, hunters_count, stars, stars_count, main_win, status_win, player);

    return res;
}
