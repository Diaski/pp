#include "game.h"
#include <stdlib.h>

int move_player(Player_t* p, Win* win,LevelConfig_t* cfg){
    if(detect_wall_collision(p->obj, win)!=0){return 0;};
    remove_from_map_obj(p->obj, win);
    p->obj.x += p->obj.dx;
    p->obj.y += p->obj.dy;
    if(p->life_force <= (cfg->player->life_force / 2)){
        p->obj.color = RED_ON_BLACK_PAIR;
    }
    change_sprite_base_on_direction(&p->obj);
    return 1;
}
void handle_player_input(Player_t* p, char input, Win* win,LevelConfig_t* cfg){
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
    }
    move_player(p, win, cfg);
    draw_to_win_and_map(p->obj, win, PLAYER_SPRITE);
}
Player_t* create_player(LevelConfig_t* config){
    Player_t* p = malloc(sizeof(Player_t));
    p->obj.width=config->player->obj.width;
    p->obj.height=config->player->obj.height;
    p->obj.x = 10;
    p->obj.y = 10;
    p->obj.speed_x = config->player->obj.speed_x;
    p->obj.speed_y = config->player->obj.speed_y;
    p->obj.dx = 0;
    p->obj.dy = p->obj.speed_y;
    p->obj.sprites_list = config->player->obj.sprites_list;
    p->obj.current_sprite = p->obj.sprites_list.down;
    p->life_force = config->player->life_force;
    p->obj.color=config->player->obj.color;
    p->stars = 0;
    return p;
}
void destroy_player(Player_t* p){
    free(p);
}
void setup_enemy_in_win(Enemy_t* enemy,Win* win){
    if(rand()%2==0){
        if(rand()%2==0){
            enemy->obj.x = 1;
            enemy->obj.y = rand()%(win->rows - enemy->obj.height -2) +1;
            enemy->obj.dx = enemy->obj.speed_x;
        } else {
            enemy->obj.x = win->cols - enemy->obj.width -1;
            enemy->obj.y = rand()%(win->rows - enemy->obj.height -2) +1;
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
void setup_enemy_data_base_on_template(Enemy_t* enemy, Enemy_t template,int time_left, LevelConfig_t* config){
    enemy->obj.width = template.obj.width;
    enemy->obj.height = template.obj.height;
    enemy->obj.speed_x = template.obj.speed_x;
    enemy->obj.speed_y = template.obj.speed_y;
    enemy->obj.sprites_list = template.obj.sprites_list;
    enemy->obj.color = template.obj.color;
    enemy->dash_limit = template.dash_limit;
    enemy->damage = calculate_damage(template.damage, config->time_limit_ms, time_left, config->damage_over_time_mult);
    enemy->alive = 1;
    enemy->sleep_after_dash = 3;
    enemy->dashing = 0;
    enemy->bounces = template.bounces;
    enemy->obj.current_sprite = enemy->obj.sprites_list.down;
}
Enemy_t* spawn_enemy(Win* win, LevelConfig_t* config,int type,int time_left){
    Enemy_t* enemy = malloc(sizeof(Enemy_t));
    Enemy_t template_enemy = config->hunters[type];
    setup_enemy_data_base_on_template(enemy, template_enemy, time_left, config);
    do{
        setup_enemy_in_win(enemy,win);
    }while(check_if_hit_player(enemy->obj, win->map));
    draw_to_map_obj(enemy->obj, win, ENEMY_SPRITE);
    return enemy;
}

void enemy_movement(Enemy_t* enemy,Player_t* player,int collision){
    if(enemy->dashing ==1 || player->obj.x > enemy->obj.x){
        if (enemy->dash_limit >0){
            enemy->dashing = dash(enemy,player);
        }
    }
    if(collision == HORIZONTAL){
        enemy->obj.dx = -enemy->obj.dx;
        if(enemy->bounces >0){
            enemy->bounces--;
        } else {
            enemy->alive =0;
            return;
        }
    }
    if(collision == VERTICAL){
        enemy->obj.dy = -enemy->obj.dy;
        if(enemy->bounces >0){
            enemy->bounces--;
        } else {
            enemy->alive =0;
            return;
        }
    }
    enemy->obj.x += enemy->obj.dx;
    enemy->obj.y += enemy->obj.dy;
}
void move_enemy(Enemy_t* enemy, Win* win, Player_t* player){
    int collision=detect_wall_collision(enemy->obj, win);
    remove_from_win_and_map(enemy->obj, win);
    enemy_movement(enemy, player, collision);
    change_sprite_base_on_direction(&enemy->obj);
    if(check_if_hit_player(enemy->obj, win->map)){
        player->life_force -= enemy->damage;
        enemy->alive = 0;
        return;
    }
    draw_to_win_and_map(enemy->obj, win, ENEMY_SPRITE);
}
void destroy_enemy_list(Enemy_t** enemy, int count){
    for (int i = 0; i < count; i++) {
        free(enemy[i]);
    }
    free(enemy);
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
    star->obj.current_sprite = malloc(sizeof(char) * 2);
    star->obj.current_sprite[0] = STAR;
    star->obj.current_sprite[1] = '\0';
    star->fade_color = YELLOW_ON_BLACK_PAIR;
}
Star_t* spawn_star(Win* win){
    Star_t* star = malloc(sizeof(Star_t));
    setup_star(star, win);
    return star;
}
void move_star(Star_t* star, Win* win,Player_t* player){
    remove_from_win_and_map(star->obj, win);
    if(win->map[star->obj.y][star->obj.x] == ENEMY_SPRITE){
        star->alive = 0;
        return;
    }
    if(check_if_hit_player(star->obj, win->map)){
        star->alive = 0;
        player->stars += 1;
        return ;
    }
    star->obj.y += 1;
    if (star->obj.y <= (win->rows/2) == 0 ) {
        star->obj.color = star->fade_color;
    }
    if(star->obj.y >= win->rows -1){
        star->alive = 0;
        return;
    }
    if(check_if_hit_player(star->obj, win->map)){
        star->alive = 0;
        player->stars += 1;
        return ;
    }
    if(win->map[star->obj.y][star->obj.x] == ENEMY_SPRITE){
        star->alive = 0;
        return;
    }
    draw_to_win_and_map(star->obj, win, STAR);
}
void free_star_list(Star_t** stars, int count){
    for (int i = 0; i < count; i++) {
        free(stars[i]->obj.current_sprite);
        free(stars[i]);
    }
    free(stars);
}