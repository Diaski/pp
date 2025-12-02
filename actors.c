#include "game.h"
#include <stdlib.h>

int move_player(Player_t* p, Win* win){
    if(detect_wall_collision(p->obj, win)!=0){return 0;};
    remove_from_map_obj(p->obj, win);
    p->obj.x += p->obj.dx;
    p->obj.y += p->obj.dy;
    if(p->obj.dx > 0){
        p->obj.current_sprite = p->obj.sprites_list.right;
        draw_to_map_obj(p->obj,win, PLAYER_SPRITE);
        return 0;
    } else if(p->obj.dx < 0){
        p->obj.current_sprite = p->obj.sprites_list.left;
        draw_to_map_obj(p->obj,win, PLAYER_SPRITE);
        return 0;
    } else if(p->obj.dy > 0){
        p->obj.current_sprite = p->obj.sprites_list.down;
        draw_to_map_obj(p->obj,win, PLAYER_SPRITE);
        return 0;
    } else if(p->obj.dy < 0){
        p->obj.current_sprite = p->obj.sprites_list.up;
        draw_to_map_obj(p->obj,win, PLAYER_SPRITE);
        return 0;
    }
    return 1;
}
void handle_player_input(Player_t* p, char input, Win* win){
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
    move_player(p, win);
}
Player_t* create_player(LevelConfig_t* config){
    Player_t* p = malloc(sizeof(Player_t));
    p->obj.width=config->player->obj.width;
    p->obj.height=config->player->obj.height;
    p->obj.x = 10;
    p->obj.y = 10;
    p->obj.speed_x = config->player->obj.speed_x;
    p->obj.speed_y = config->player->obj.speed_y;
    p->obj.sprites_list = config->player->obj.sprites_list;
    p->obj.current_sprite = p->obj.sprites_list.down;
    p->life_force = config->player->life_force;
    p->obj.color=config->player->obj.color;
    return p;
}
void destroy_player(Player_t* p){
    free(p);
}
void enemy_random_data(Enemy_t* enemy,Win* win){
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
int check_if_hit_player(Enemy_t* enemy,Map_t map){
    for(int row = 0; row < enemy->obj.height; row++){
        for(int col = 0; col < enemy->obj.width; col++){
            if(map[enemy->obj.y + row][enemy->obj.x + col] == PLAYER_SPRITE){
                return 1;
            }
        }
    }
    return 0;
}
Enemy_t* spawn_enemy(Win* win, LevelConfig_t* config,int type){
    Enemy_t* enemy = malloc(sizeof(Enemy_t));
    Enemy_t temp_enemy = config->hunters[type];
    enemy->obj.width = temp_enemy.obj.width;
    enemy->obj.height = temp_enemy.obj.height;
    enemy->obj.speed_x = temp_enemy.obj.speed_x;
    enemy->obj.speed_y = temp_enemy.obj.speed_y;
    enemy->obj.sprites_list = temp_enemy.obj.sprites_list;
    enemy->obj.color = temp_enemy.obj.color;
    enemy->damage = temp_enemy.damage;
    enemy->alive = 1;
    enemy->bounces = temp_enemy.bounces;
    enemy->obj.current_sprite = enemy->obj.sprites_list.down;
    enemy_random_data(enemy,win);
    while(check_if_hit_player(enemy, win->map)){
        enemy_random_data(enemy,win);
    }
    calculate_damage(enemy, config->time_limit_ms, 0, config->damage_over_time_mult);
    draw_to_map_obj(enemy->obj, win, ENEMY_SPRITE);
    return enemy;
}
void move_enemy(Enemy_t* enemy, Win* win, Player_t* player){
    int collision=detect_wall_collision(enemy->obj, win);
    remove_from_map_obj(enemy->obj, win);
    remove_obj_from_window(enemy->obj, win);
    if(collision){
        if(enemy->bounces-1 > 0){
            if(collision == HORIZONTAL){
                enemy->obj.dx = -enemy->obj.dx;
                collision = detect_wall_collision(enemy->obj, win);
            }
            if (collision == VERTICAL){
                enemy->obj.dy = -enemy->obj.dy;
            }
            enemy->bounces--;
        } else {
            enemy->alive = 0;
            return;
        }
    }
    enemy->obj.x += enemy->obj.dx;
    enemy->obj.y += enemy->obj.dy;
    if(enemy->obj.dx > 0){
        enemy->obj.current_sprite = enemy->obj.sprites_list.right;
    } else if(enemy->obj.dx < 0){
        enemy->obj.current_sprite = enemy->obj.sprites_list.left;
    } else if(enemy->obj.dy > 0){
        enemy->obj.current_sprite = enemy->obj.sprites_list.down;
    } else if(enemy->obj.dy < 0){
        enemy->obj.current_sprite = enemy->obj.sprites_list.up;
    }
    if(check_if_hit_player(enemy, win->map)){
        player->life_force -= enemy->damage;
        enemy->alive = 0;
        return;
    }
    draw_to_map_obj(enemy->obj,win, ENEMY_SPRITE);
    draw_obj(enemy->obj, win);
}
void destroy_enemy_list(Enemy_t** enemy, int count){
    for (int i = 0; i <= count; i++) {
        free(enemy[i]);
    }
    free(enemy);
}