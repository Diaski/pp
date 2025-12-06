#include "game.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


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
        case 'w':
            p->obj.dx = 0;
            p->obj.dy = -p->obj.speed_y;
            break;
        case 's':
            p->obj.dx = 0;
            p->obj.dy = p->obj.speed_y;
            break;
        case 'a':
            p->obj.dx = -p->obj.speed_x;
            p->obj.dy = 0;
            break;
        case 'd':
            p->obj.dx = p->obj.speed_x;
            p->obj.dy = 0;
            break;
        case 'e':
            if(p->taxi_cooldown <=0){
                taxi(p,win,game_speed,hunters,hunters_count,stars,stars_count);
                p->taxi_cooldown = p->base_taxi_cooldown;
            }
            break;
        default: break;
    }
    if(p->taxi_cooldown >0){
        int cd =(((p->taxi_cooldown*game_speed)-game_speed)/game_speed);
        p->taxi_cooldown = cd;
    }
    if(p->life_force <= (cfg->player->life_force / 2)){
        p->obj.color = RED_ON_BLACK_PAIR;
    }
    move_player(p, win,0);
    draw_to_win_and_map(p->obj, win, PLAYER_SPRITE);
}
int move_player(Player_t* p, Win* win,int dont_change_sprite){
    remove_from_map_obj(p->obj, win);
    reverse_when_touch_border(&p->obj, win);
    p->obj.x += p->obj.dx;
    p->obj.y += p->obj.dy;
    if(dont_change_sprite){
        return 1;
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


void destroy_player(Player_t* p){
    free(p);
}
void setup_enemy_in_win(Enemy_t* enemy,Win* win){
    if(rand()%2==0){
        if(rand()%2==0){
            enemy->obj.x = 1;
            enemy->obj.y = rand()%(win->rows - enemy->obj.height -2)+1;
            enemy->obj.dx = enemy->obj.speed_x;
        } else {
            enemy->obj.x = win->cols - enemy->obj.width -1;
            enemy->obj.y = rand()%(win->rows - enemy->obj.height -2)+1;
            enemy->obj.dx = -enemy->obj.speed_x;
        }
        enemy->obj.dy = 0;
    } else {
        if(rand()%2==0){
            enemy->obj.y = 1;
            enemy->obj.x = rand()%(win->cols - enemy->obj.width -2) +1;
            enemy->obj.dy = enemy->obj.speed_y;
        } else {
            enemy->obj.y = win->rows - enemy->obj.height -1;
            enemy->obj.x = rand()%(win->cols - enemy->obj.width -2) +1;
            enemy->obj.dy = -enemy->obj.speed_y;
        }
        enemy->obj.dx = 0;
    }
}
void setup_enemy_data_base_on_template(Enemy_t* enemy, Enemy_t temp,int time_left, LevelConfig_t* config){
    enemy->obj.width = temp.obj.width;
    enemy->obj.height = temp.obj.height;
    enemy->obj.speed_x = temp.obj.speed_x;
    enemy->obj.speed_y = temp.obj.speed_y;
    enemy->obj.sprites_list = temp.obj.sprites_list;
    enemy->obj.color = temp.obj.color;
    enemy->dash_limit = temp.dash_limit;
    enemy->damage = calculate_damage(temp.damage, config->time_limit_ms, time_left, config->damage_over_time_mult);
    enemy->alive = 1;
    enemy->sleep_after_dash = 3;
    enemy->dashing = 0;
    enemy->bounces = calculate_bounces(temp.bounces, config->time_limit_ms, time_left);
    enemy->obj.current_sprite = enemy->obj.sprites_list.down;
    enemy->base_dash_sleep_time = temp.base_dash_sleep_time;
}
Enemy_t* spawn_enemy(Win* win, LevelConfig_t* config,int type,int time_left){
    Enemy_t* enemy = (Enemy_t*)calloc(1,sizeof(Enemy_t));
    const Enemy_t template_enemy = config->hunters[type];
    setup_enemy_data_base_on_template(enemy, template_enemy, time_left, config);

    do{
        setup_enemy_in_win(enemy,win);
    }while(check_if_hit_player(enemy->obj, win));

    draw_to_map_obj(enemy->obj, win, ENEMY_SPRITE);
    return enemy;
}
int reverse_when_touch_border(GameObject_t* obj, Win* win){
    int jumped=0;
    int collision = detect_wall_collision(*obj, win);
    if(collision == HORIZONTAL){
        obj->dx = -obj->dx;
        jumped=1;
    }
    collision = detect_wall_collision(*obj, win);
    if(collision == VERTICAL){
        obj->dy = -obj->dy;
        jumped=1;
    }
    return jumped;
}
void enemy_movement(Enemy_t* enemy,Player_t* player,Win* win){
    if(check_if_missed_player(&player->obj, &enemy->obj) || enemy->dashing){
        if (enemy->dash_limit >0){
            enemy->dashing = dash(enemy,player);
        }
    }
    if(reverse_when_touch_border(&enemy->obj, win)){
        if(enemy->bounces >0){
            enemy->bounces -= 1;
        }
        else {
            enemy->alive = 0;
            return;
        }
    }
    enemy->obj.x += enemy->obj.dx;
    enemy->obj.y += enemy->obj.dy;
}
void move_enemy(Enemy_t* enemy, Win* win, Player_t* player,int only_draw){
    if(only_draw){
        draw_to_win_and_map(enemy->obj, win, ENEMY_SPRITE);
        return;
    }
    remove_from_win_and_map(enemy->obj, win);
    enemy_movement(enemy, player, win);
    change_sprite_base_on_direction(&enemy->obj);
    if(check_if_hit_player(enemy->obj, win)&& enemy->alive ==1){
        player->life_force -= enemy->damage;
        enemy->alive = 0;
        return;
    }
    if(enemy->alive ==0){
        return;
    }
    draw_to_win_and_map(enemy->obj, win, ENEMY_SPRITE);
}
void destroy_enemy_list(Enemy_t** enemy, int count){
    for (int i = 0; i < count; i++) {
        free((void*)enemy[i]);
    }
    free((void*)enemy);
}
void setup_star(Star_t* star, Win* win){
    star->obj.width = 1;
    star->obj.height = 1;
    star->obj.x = rand() % (win->cols - 2) + 1;
    star->obj.y = 1;
    star->obj.dx = 0;
    star->obj.dy = 1;
    star->obj.color = WHITE_ON_BLACK_PAIR;
    star->alive = 1;
    star->obj.current_sprite = (Sprite_t)calloc(2,sizeof(char));
    star->obj.current_sprite[0] = STAR;
    star->obj.current_sprite[1] = '\0';
    star->fade_color = YELLOW_ON_BLACK_PAIR;
}
Star_t* spawn_star(Win* win){
    Star_t* star = (Star_t*)calloc(1,sizeof(Star_t));
    setup_star(star, win);
    return star;
}
void check_star_hit(Star_t* star, Win* win,Player_t* player){
    if(check_if_hit_player(star->obj, win)){
        star->alive = 0;
        player->stars += 1;
        return ;
    }
    if(win->map[star->obj.y][star->obj.x] == ENEMY_SPRITE){
        star->alive = 0;
    }
    if(star->obj.y >= win->rows -1){
        star->alive = 0;
        return;
    }
}
void move_star(Star_t* star, Win* win,Player_t* player,int only_draw){
    if(only_draw){
        draw_to_win_and_map(star->obj, win, STAR);
        return;
    }
    check_star_hit(star, win, player);
    if(star->alive ==0){
        return;
    }
    remove_from_win_and_map(star->obj, win);
    if(check_if_star_hit_player(star->obj, win)){
        star->alive = 0;
        player->stars += 1;
    }
    if(star->alive ==0){
        return;
    }
    star->obj.y += star->obj.dy;
    if (star->obj.y >= (win->rows/STAR_FADE_CONVERSION_RATE)) {
        star->obj.color = star->fade_color;
    }
    check_star_hit(star, win, player);
    if(star->alive ==0){
        return;
    }
    draw_to_win_and_map(star->obj, win, STAR);
}
void while_in_dash_logic(Player_t* p, Win* win,int game_speed, Enemy_t** hunters,int hunters_count, Star_t** stars,int stars_count,int x,int y){
    if(p->obj.x == x){
        p->obj.dx =0;
    }
    if(p->obj.y == y){
        p->obj.dy =0;
    }
    move_player(p, win,1);
    draw_obj(p->obj, win);
    move_enemy_list(&hunters, hunters_count, win, p,1);
    move_star_list(stars, stars_count, win, p,1);
    mount_upd(win);
    doupdate();
    remove_from_win_and_map(p->obj,win);
    usleep((__useconds_t)game_speed* USC_T_MSC);
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
    Sprite_t old_sprite= (Sprite_t)calloc((unsigned int)(strlen(p->obj.current_sprite)+1),sizeof(char));
    strcpy(old_sprite, p->obj.current_sprite);
    int old_width = p->obj.width;
    int old_height = p->obj.height;
    int prev_dy= p->obj.dy;
    int prev_dx= p->obj.dx;
    do{
        x = rand() % (win->cols - p->obj.width -2) +p->obj.width+1;
        y = rand() % (win->rows - p->obj.height -2) +p->obj.height+1;
    }while(detect_if_spot_hunter(win, x, y, p->obj.width, p->obj.height));
    remove_from_win_and_map(p->obj,win);
    convert_player_to_taxi_sprite(p, x, y);
    while(p->obj.x != x && p->obj.y != y){
        while_in_dash_logic(p, win, game_speed, hunters, hunters_count, stars, stars_count, x, y);
    }
    strcpy(p->obj.current_sprite, old_sprite);
    p->obj.width = old_width;
    p->obj.height = old_height;
    p->obj.dx = prev_dx;
    p->obj.dy = prev_dy;
    free(old_sprite);
}
void free_star_list(Star_t** stars, int count){
    for (int i = 0; i < count; i++) {
        free((void*)stars[i]->obj.current_sprite);
        free(stars[i]);
    }
    free((void*)stars);
}