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
void dash_calculate(Enemy_t* enemy,Player_t* player){
    if (player->obj.x > enemy->obj.x){
        enemy->obj.dx = enemy->obj.speed_x *2;
    } else if (player->obj.x < enemy->obj.x){
        enemy->obj.dx = -enemy->obj.speed_x *2;
    } else {
        enemy->obj.dx = 0;
    }
    if (player->obj.y > enemy->obj.y){
        enemy->obj.dy = enemy->obj.speed_y *2;
    } else if (player->obj.y < enemy->obj.y){
        enemy->obj.dy = -enemy->obj.speed_y *2;
    } else {
        enemy->obj.dy = 0;
    }
    enemy->dashing =0;
    enemy->sleep_after_dash =3;
    enemy->dash_limit--;
}
int dash(Enemy_t* enemy,Player_t* player){
    if(enemy->sleep_after_dash >0){
        enemy->obj.dx = 0;
        enemy->obj.dy = 0;
        enemy->sleep_after_dash--;
        return 1;
    }
    if (enemy->sleep_after_dash ==0)dash_calculate(enemy,player);
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