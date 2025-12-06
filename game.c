#include "game.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void random_enemy_spawn(Win* win, LevelConfig_t* config, Enemy_t** hunters, int* hunters_c,int time_left){
    if(rand()%config->hunter_spawn_rate==0 && *hunters_c < config->max_enemys_per_level){
        Enemy_t* new_enemy=spawn_enemy(win, config, rand()%(config->hunter_type_count), time_left);
        if(new_enemy != NULL){
            hunters[*hunters_c]=new_enemy;
            (*hunters_c)++;
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
void move_star_list(Star_t** stars, int count, Win* win, Player_t* player,int only_draw){
    for (int i = 0; i < count; i++) {
        if(stars[i]->alive){
            move_star(stars[i], win, player, only_draw);
        }
    }
}
void move_enemy_list(Enemy_t*** enemy, int count, Win* win, Player_t* player,int only_draw){
    for (int i = 0; i < count; i++) {
        if((*enemy)[i]->alive){
            move_enemy((*enemy)[i], win, player, only_draw);
        }
    }
}
void free_all_from_level(LevelConfig_t* config, Enemy_t** hunters, int hunters_count, Star_t** stars, int stars_count, Win* main_win, Win* status_win, Player_t* player){
    destroy_player(player);
    destroy_enemy_list(hunters, hunters_count);
    free_star_list(stars, stars_count);
    free_map(main_win->map, main_win->rows);
    del_window(main_win);
    del_window(status_win);
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
void end_of_tick(int game_speed,int* time_left){
    usleep((__useconds_t)(game_speed * USC_T_MSC));
    *time_left -= game_speed;
}
int game_loop(Win* main_win, Win* status_win, Player_t* p, int g_speed,LevelConfig_t* cfg,Enemy_t** hunters,int* hunters_count,int level_num, Star_t** stars,int* stars_count){
    char input,last_input;
    if (hunters == NULL) {
        return(MALLOC_ERROR);
    }
    nodelay(stdscr, TRUE);
    int time_left = cfg->time_limit_ms;
    while (1)
    {   
        last_input = (char)getch();
        if(last_input != ERR){       
            input = last_input;
        }
        flushinp();
        if(validate_input(input, p, &g_speed, cfg, time_left)){
            return PLAYER_DIED;
        }

        handle_player_input(p, input,main_win,cfg, g_speed,hunters,*hunters_count, stars,*stars_count);

        random_star_spawn(main_win, cfg, stars, stars_count);
        move_star_list(stars, *stars_count, main_win, p, 0);

        random_enemy_spawn(main_win, cfg, hunters, hunters_count, time_left);
        move_enemy_list(&hunters, *hunters_count, main_win, p,0);

        update_status_display(status_win, p->name,level_num,time_left,p->stars,cfg->star_quota,p->life_force);
        wnoutrefresh(main_win->window);
        mount_upd(status_win);
        doupdate();

        input=0;
        end_of_tick(g_speed, &time_left);
        if(p->stars >= cfg->star_quota){
            return 0;
        }
    }
    return 1;
}
int check_config(LevelConfig_t* config){
    int res=0;
    if (config == NULL) {
        res = LOADING_CONFIG_ERROR;
    }
    if(config->max_enemys_per_level <0 ){
        res = LOADING_CONFIG_ERROR;
    }
    if(LINES < config->map_rows + config->status_rows || COLS < config->map_cols){
        res = LOADING_CONFIG_ERROR;
    }
    if(config->player == NULL){
        res = LOADING_CONFIG_ERROR;
    }
    if(config->hunters == NULL){
        res = LOADING_CONFIG_ERROR;
    }
    if(res !=0){
        free_level_config(config);
        clear();
        refresh();
    }
    return res;
}

void end_game(char* player_name, int level_num, int res, int score){
    if(res == 0){
        congratulate_player_win(player_name, level_num);
    } else {
        printf("Level failed or exited.\n");
    }
    save_to_leaderboard(player_name, score);
    show_leaderboard();
}
Win* s_win_create(LevelConfig_t* cfg,int x,int y,int have_map){ 
    return create_window(cfg->map_rows,cfg->map_cols,x,y,have_map); //function to Make level selector cleaner for not writitng so many lines
}
int level_selector(char* p_name){
    const int level = display_level_selection_menu();
    if (level == -1){
        return EXIT_GAME;
    }

    LevelConfig_t* cfg = load_level_config(level);
    int res = check_config(cfg);
    if (res != 0){
        return res;
    }

    Enemy_t** hunters=(Enemy_t**)calloc((unsigned int)cfg->max_enemys_per_level,sizeof(Enemy_t*));
    Star_t** stars=(Star_t**)calloc((unsigned int)cfg->max_enemys_per_level,sizeof(Star_t*));
    Win* main_win=create_window(cfg->map_rows,cfg->map_cols,0,0,1);
    Win* status_win=create_window(cfg->status_rows,cfg->status_cols,0,cfg->map_rows,0);
    Player_t* p=create_player(cfg);
    int stars_count=0;
    int hunters_count=0;
    const int game_speed = cfg->min_speed;
    strncpy(p->name, p_name, PLAYER_NAME_MAX - 1);
    
    refresh();

    mount_upd(main_win);
    mount_upd(status_win);
    wnoutrefresh(main_win->window);
    doupdate();

    if(res==0) res = game_loop(main_win,status_win,p, game_speed,cfg,hunters,&hunters_count, level, stars, &stars_count);
    
    end_game(p_name, level, res, calculate_score(cfg, p, cfg->time_limit_ms, 0));
    free_all_from_level(cfg, hunters, hunters_count, stars, stars_count, main_win, status_win, p);
    return res;
}
