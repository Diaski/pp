#include "game.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int calculate_score(LevelConfig_t* level_config,Player_t* player,int time_max, int time_left) {
    const int score_from_time = (int)(((long long)level_config->score_time_bias * time_max) / (time_max + time_left));
    const int score_from_stars = level_config->score_star_bias * player->stars;
    if(player->life_force <=0){
        return 0;
    }
    return score_from_time * score_from_stars * player->life_force/LIFE_FORCE_TO_SCORE_DIVIDER * level_config->base_score;
}
Player_t* create_player(LevelConfig_t* config){
    Player_t* p = (Player_t*)calloc(1,sizeof(Player_t));
    p->obj.width=config->player->obj.width;
    p->obj.height=config->player->obj.height;
    p->obj.x = PLAYER_BASE_SPAWN_X;
    p->obj.y = PLAYER_BASE_SPAWN_Y;
    p->obj.speed_x = config->player->obj.speed_x;
    p->obj.speed_y = config->player->obj.speed_y;
    p->obj.dx = 0;
    p->obj.dy = p->obj.speed_y;
    p->obj.sprites_list = config->player->obj.sprites_list;
    p->obj.current_sprite = p->obj.sprites_list.down;
    p->life_force = config->player->life_force;
    p->obj.color=config->player->obj.color;
    p->stars = 0;
    p->taxi_cooldown = 0;
    p->base_taxi_cooldown = config->player->base_taxi_cooldown;
    p->blink_sprites.left = config->player->blink_sprites.left;
    p->blink_sprites.right = config->player->blink_sprites.right;
    p->blink_sprites.up = config->player->blink_sprites.up;
    p->blink_sprites.down = config->player->blink_sprites.down;
    p->blink_tick = 0;
    p->blink_tick_ratio = config->player->blink_tick_ratio;
    return p;
}

void handle_player_input(Player_t* p, char input, Win* win,LevelConfig_t* cfg,int game_speed, Enemy_t** hunters,int hunters_count, Star_t** stars,int stars_count){
    remove_obj_from_window(p->obj, win);
    switch(input){
        case K_UP:
            p->obj.dx = 0;
            p->obj.dy = -p->obj.speed_y;
            break;
        case K_DOWN:
            p->obj.dx = 0;
            p->obj.dy = p->obj.speed_y;
            break;
        case K_LEFT:
            p->obj.dx = -p->obj.speed_x;
            p->obj.dy = 0;
            break;
        case K_RIGHT:
            p->obj.dx = p->obj.speed_x;
            p->obj.dy = 0;
            break;
        case K_TAXI:
            if(p->taxi_cooldown <=0){
                taxi(p,win,game_speed,hunters,hunters_count,stars,stars_count);
                p->taxi_cooldown = p->base_taxi_cooldown;
            }
            break;
        default: break;
    }
    if(p->taxi_cooldown >0){
        const int cd =(((p->taxi_cooldown*game_speed)-game_speed)/game_speed);
        p->taxi_cooldown = cd;
    }
    if(p->life_force <= (cfg->player->life_force / PLAYER_CHANGE_COLOR_THRESHOLD)){
        p->obj.color = RED_ON_BLACK_PAIR;
    }
    move_player(p, win,false);
    draw_to_win_and_map(p->obj, win, PLAYER_SPRITE);
}

int move_player(Player_t* p, Win* win,int dont_change_sprite){
    remove_from_map_obj(p->obj, win);
    reverse_when_touch_border(&p->obj, win);
    p->obj.x += p->obj.dx;
    p->obj.y += p->obj.dy;
    if(dont_change_sprite){
        return true;
    }
    if(p->blink_tick < p->blink_tick_ratio){
        change_sprite_base_on_direction(&p->obj);
        p->blink_tick += 1;
    } else {
        change_blink_sprite_base_on_direction(p);
        p->blink_tick = 0;
    }
    return 1;
}

void while_in_dash_logic(Player_t* p, Win* win,int game_speed, Enemy_t** hunters,int hunters_count, Star_t** stars,int stars_count,int x,int y){
    if(p->obj.x == x){
        p->obj.dx =0;
    }
    if(p->obj.y == y){
        p->obj.dy =0;
    }
    move_player(p, win,true);
    draw_obj(p->obj, win);
    move_enemy_list(&hunters, hunters_count, win, p,true);
    move_star_list(stars, stars_count, win, p,true);
    wnoutrefresh(win->window);
    doupdate();
    remove_from_win_and_map(p->obj,win);
    usleep(game_speed* USC_T_MSC);
}

void convert_player_to_taxi_sprite(Player_t* p, int x, int y){
    strcpy(p->obj.current_sprite, TAXI_SPRITE);
    p->obj.width = TAXI_WIDTH;
    p->obj.height = TAXI_HEIGHT;
    if(x-p->obj.x>0){
        p->obj.dx = TAXI_SPEED;
    } 
    else if (x-p->obj.x<0){
        p->obj.dx = -TAXI_SPEED;
    }
    else {
        p->obj.dx =0;
    }
    if(y-p->obj.y>0){
        p->obj.dy = TAXI_SPEED;
    } 
    else if (y-p->obj.y<0){
        p->obj.dy = -TAXI_SPEED;
    }
    else {
        p->obj.dy =0;
    }
}

void taxi(Player_t* p, Win* win,int game_speed, Enemy_t** hunters,int hunters_count, Star_t** stars,int stars_count){
    int x=0,y=0;
    int count=0;
    char *old_sprite = (char*)calloc(strlen(p->obj.current_sprite) + 1,sizeof(char));
    strcpy(old_sprite, p->obj.current_sprite);
    const int old_width = p->obj.width;
    const int old_height = p->obj.height;
    const int prev_dy= p->obj.dy;
    const int prev_dx= p->obj.dx;
    int mx = (p->obj.width > TAXI_WIDTH) ? p->obj.width : TAXI_WIDTH;
    int my = (p->obj.height > TAXI_HEIGHT) ? p->obj.height : TAXI_HEIGHT;

    do{
        x = rand() % (win->cols - mx - 2) + mx + 1;
        y = rand() % (win->rows - my - 2) + my + 1;
    }while(find_safe_spot(win, x, y, p) && ++count <= MAX_TAXI_SPAWN_ATTEMPTS);

    if(count <= MAX_TAXI_SPAWN_ATTEMPTS){
        remove_from_win_and_map(p->obj,win);
        convert_player_to_taxi_sprite(p, x, y);
        while((p->obj.x != x) || (p->obj.y != y)){
            while_in_dash_logic(p, win, game_speed, hunters, hunters_count, stars, stars_count, x, y);
        }
    }
    strcpy(p->obj.current_sprite, old_sprite);
    p->obj.width = old_width;
    p->obj.height = old_height;
    p->obj.dx = prev_dx;
    p->obj.dy = prev_dy;
    free(old_sprite);
}

void create_save(char** save,LevelConfig_t* cfg){
    if (*save != NULL) {
        free(*save);
    }
    *save = (char*)calloc((unsigned int)(cfg->max_speed*cfg->time_limit_ms+2), sizeof(char));  // 2 one for null second for level number
    if (*save==NULL){
        exit(MALLOC_ERROR);
    }
    return;
}

void destroy_player(Player_t* p){
    free(p);
}