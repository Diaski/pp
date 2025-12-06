#include "game.h"
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//setup level, run game loop, and cleanup after level
int level_selector(char* p_name){
    int stars_count=0;
    int hunters_count=0;
    int time_left=0;
    int g_speed;
    const int lvl = display_level_selection_menu();
    if (lvl == EXIT_GAME){
        return EXIT_GAME;
    }

    LevelConfig_t* cfg = load_level_config(lvl);
    int res = check_config(cfg);
    if (res != 0){
        return res;
    }
    g_speed = cfg->max_speed;
    Enemy_t** hunters=(Enemy_t**)calloc((unsigned int)cfg->max_enemys,sizeof(Enemy_t*));
    Star_t** stars=(Star_t**)calloc((unsigned int)cfg->max_enemys,sizeof(Star_t*));

    Win* main_win=create_window(cfg->map_rows,cfg->map_cols,0,0,1);
    Win* status_win=create_window(cfg->status_rows,cfg->status_cols,0,cfg->map_rows,0);
    Player_t* p=create_player(cfg);
    strncpy(p->name, p_name, PLAYER_NAME_MAX - 1);
    // -1 to leave space for null terminator
    refresh();

    wnoutrefresh(main_win->window);
    wnoutrefresh(status_win->window);
    doupdate();

    if(!res) res = game_loop(main_win,status_win,p, g_speed,cfg,hunters,&hunters_count, lvl, stars, &stars_count, &time_left);
    
    end_level(p_name, lvl, res, calculate_score(cfg, p, cfg->time_limit_ms, time_left));
    free_all_from_level(cfg, hunters, hunters_count, stars, stars_count, main_win, status_win, p);
    return res;
}

int game_loop(Win* main_win, Win* status_win, Player_t* p, int g_speed,LevelConfig_t* cfg,Enemy_t** hunters,int* hunters_count,int level_num, Star_t** stars,int* stars_count,int* time_left){
    char input,last_input;
    if (hunters == NULL) {
        return(MALLOC_ERROR);
    }
    nodelay(stdscr, TRUE);
    *time_left = cfg->time_limit_ms;
    while (true)
    {   
        last_input = (char)getch();
        if(last_input != ERR){       
            input = last_input;
        }
        flushinp();
        if(validate_input(input, p, &g_speed, cfg, *time_left)){
            return PLAYER_DIED;
        }
        //validate input(only for game settings like exiting and speed change) and flushinput buffer to avoid input lag

        handle_player_input(p, input,main_win,cfg, g_speed,hunters,*hunters_count, stars,*stars_count);

        random_star_spawn(main_win, cfg, stars, stars_count);
        move_star_list(stars, *stars_count, main_win, p, false);

        random_enemy_spawn(main_win, cfg, hunters, hunters_count, *time_left);
        move_enemy_list(&hunters, *hunters_count, main_win, p,false);

        update_status_display(status_win, p->name,level_num,*time_left,p->stars,cfg->star_quota,p->life_force);
        wnoutrefresh(main_win->window);
        wnoutrefresh(status_win->window);
        doupdate();

        input=0;
        end_of_tick(g_speed, time_left);
        if(p->stars >= cfg->star_quota){
            break;
        }
    }
    clear();
    refresh();
    return 0;
}
//basic validation of input for settings and force exit condition
int validate_input(char input, Player_t* player, int* game_speed, LevelConfig_t* config, int time_left){
    if(input == K_QUIT || player->life_force <= 0 || time_left <= 0){
        return EXIT_GAME;
    }
    if(input == K_SPEED_UP && *game_speed -config->delta_speed >= config->max_speed){
        *game_speed -= config->delta_speed;
    }
    if(input == K_SPEED_DOWN && *game_speed +config->delta_speed <= config->min_speed){
        *game_speed += config->delta_speed;
    }
    return 0;
}

void end_of_tick(int game_speed,int* time_left){
    usleep((game_speed * USC_T_MSC));
    *time_left -= game_speed;
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

void end_level(char* player_name, int level_num, int res, int score){
    if(!res || res == EXIT_GAME|| res == PLAYER_DIED){
        congratulate_player_win(player_name, level_num,res);
    }
    save_to_leaderboard(player_name, score);
    show_leaderboard();
}