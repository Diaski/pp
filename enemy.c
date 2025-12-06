#include "game.h"
#include <stdlib.h>
#include <unistd.h>

Enemy_t* spawn_enemy(Win* win, LevelConfig_t* config,int type,int time_left){
    Enemy_t* enemy = (Enemy_t*)calloc(1,sizeof(Enemy_t));
    const Enemy_t template_enemy = config->hunters[type];
    setup_enemy_data_base_on_template(enemy, template_enemy, time_left, config);

    do{
        setup_enemy_in_window(enemy,win);
    }while(check_if_hit_player(enemy->obj, win));

    draw_to_map_obj(enemy->obj, win, ENEMY_SPRITE);
    return enemy;
}

//move all enemies in the list
void move_enemy_list(Enemy_t*** enemy, int count, Win* win, Player_t* player,int only_draw){
    for (int i = 0; i < count; i++) {
        if((*enemy)[i]->alive){
            move_enemy((*enemy)[i], win, player, only_draw);
        }
    }
}
//randomly spawn enemy based on config settings
void random_enemy_spawn(Win* win, LevelConfig_t* config, Enemy_t** hunters, int* hunters_c,int time_left){
    if(rand()%config->hunter_spawn_rate==0 && *hunters_c < config->max_enemys){
        Enemy_t* new_enemy=spawn_enemy(win, config, rand()%(config->hunter_type_count), time_left);
        if(new_enemy != NULL){
            hunters[*hunters_c]=new_enemy;
            (*hunters_c)++;
        }
    }

}
//damege increase over time
int calculate_damage(int damage, int time_max, int time_left, int dmg_mul) {
    const long long top = (long long)damage * time_max * dmg_mul;
    const long long bottom = time_max + time_left;
    return (int)(top / bottom);
}
//bounces increase over time
int calculate_bounces(int bounces, int time_max, int time_left) {
    const long long top = (long long)bounces * time_max * BOUNCES_MULT_OVER_TIME;
    const long long bottom = time_max + time_left;
    
    return (int)(top / bottom);
}
//setup enemy in random position next to window border giving same chance to all sides and set movement direction towards inside of the window
void setup_enemy_in_window(Enemy_t* enemy,Win* win){
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
    enemy->alive = ALIVE;
    enemy->sleep_after_dash = temp.base_dash_sleep_time;
    enemy->dashing = false;
    enemy->bounces = calculate_bounces(temp.bounces, config->time_limit_ms, time_left);
    enemy->obj.current_sprite = enemy->obj.sprites_list.down;
    enemy->base_dash_sleep_time = temp.base_dash_sleep_time;
}

void enemy_movement(Enemy_t* enemy,Player_t* player,Win* win){
    if(check_if_missed_player(&player->obj, &enemy->obj) || enemy->dashing){
        if (enemy->dash_limit >0){
            enemy->dashing = dash(enemy,player);//dash function handles sleep after dash
        }
    }
    if(reverse_when_touch_border(&enemy->obj, win)){
        if(enemy->bounces >0){
            enemy->bounces += BOUNCES_CHANGE;
        }
        else {
            enemy->alive = DEAD;
            return;
        }
    }
    enemy->obj.x += enemy->obj.dx;
    enemy->obj.y += enemy->obj.dy;
}
//move single enemy and check for collision with player can also just draw if only_draw is set
void move_enemy(Enemy_t* enemy, Win* win, Player_t* player,int only_draw){
    if(only_draw){
        draw_to_win_and_map(enemy->obj, win, ENEMY_SPRITE);
        return;
    }
    if(check_if_hit_player(enemy->obj, win)&& enemy->alive ==ALIVE){
        player->life_force -= enemy->damage;
        enemy->alive = DEAD;
        remove_from_win_and_map(enemy->obj, win);
        return;
    }
    remove_from_win_and_map(enemy->obj, win);
    enemy_movement(enemy, player, win);
    change_sprite_base_on_direction(&enemy->obj);
    if(check_if_hit_player(enemy->obj, win)&& enemy->alive ==ALIVE){
        player->life_force -= enemy->damage;
        enemy->alive = DEAD;
        return;
    }
    if(enemy->alive ==DEAD){
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
//calculate dash direction and set speed accordingly, also set sleep after dash and decrease dash limit
//enemy is dashing for quorter of win where player is located
void dash_calculate(Enemy_t* enemy,Player_t* player){
    if (player->obj.x > enemy->obj.x){
        enemy->obj.dx = enemy->obj.speed_x *DASH_SPEED_MULTIPLIER;
    } else if (player->obj.x < enemy->obj.x){
        enemy->obj.dx = -enemy->obj.speed_x *DASH_SPEED_MULTIPLIER;
    }
    else{
        enemy->obj.dx =0;
    }
    if (player->obj.y > enemy->obj.y){
        enemy->obj.dy = enemy->obj.speed_y *DASH_SPEED_MULTIPLIER;
    } else if (player->obj.y < enemy->obj.y){
        enemy->obj.dy = -enemy->obj.speed_y *DASH_SPEED_MULTIPLIER;
    }
    else{
        enemy->obj.dy =0;
    }
    enemy->dashing = false;
    enemy->sleep_after_dash =enemy->base_dash_sleep_time;
    enemy->dash_limit--;
}
//handle dash logic and sleep after dash
int dash(Enemy_t* enemy,Player_t* player){
    if(enemy->sleep_after_dash >0){
        enemy->obj.dx = 0;
        enemy->obj.dy = 0;
        enemy->sleep_after_dash--;
        enemy->dashing = false;
        return true;
    }
    if (enemy->sleep_after_dash <= 0)dash_calculate(enemy,player);
    return false;
}