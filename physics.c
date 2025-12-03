#include "game.h"

int detect_wall_collision(GameObject_t obj, Win* win){
    if(obj.dx >0){
        if(obj.x + obj.width + obj.dx >= win->cols ){
            return HORIZONTAL;
        }
    }
    if(obj.dx <0){
        if(obj.x + obj.dx <= 0){
            return HORIZONTAL;
        }
    }
    if(obj.dy >0){
        if(obj.y + obj.height + obj.dy >= win->rows ){
            return VERTICAL;
        }
    }
    if(obj.dy <0){
        if(obj.y + obj.dy <= 0){
            return VERTICAL;
        }
    }
    return 0;
}
void dash_calculate(Enemy_t* enemy, Player_t* player){
    if(enemy->dashing ==0){
        enemy->wanted_x = player->obj.x;
        enemy->wanted_y = player->obj.y;
        enemy->obj.dx = (player->obj.x - enemy->obj.x)/abs(player->obj.x - enemy->obj.x) * enemy->obj.speed_x;
        enemy->obj.dy = (player->obj.y - enemy->obj.y)/abs(player->obj.y - enemy->obj.y) * enemy->obj.speed_y;
    }
    enemy->dashing =1;
}
int dash_to_player(Enemy_t* enemy, Player_t* player){
    if(enemy->sleep_after_dash >0){
        enemy->sleep_after_dash--;
        return 1;
    }
    if(enemy->obj.x == enemy->wanted_x && enemy->obj.y == enemy->wanted_y ){
        enemy->obj.dx = 0;
        enemy->obj.dy = 0;
        enemy->dashing = 0;
        enemy->sleep_after_dash = 3;
        return 0;
    }
    if (enemy->dashing ==0)dash_calculate(enemy, player);
    return 0;
}

int check_if_hit_player(GameObject_t obj,Map_t map){
    for(int row = 0; row < obj.height; row++){
        for(int col = 0; col < obj.width; col++){
            if(map[obj.y + row][obj.x + col] == PLAYER_SPRITE){
                return 1;
            }
        }
    }
    return 0;
}