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
    }
    else{
        enemy->obj.dx =0;
    }
    if (player->obj.y > enemy->obj.y){
        enemy->obj.dy = enemy->obj.speed_y *2;
    } else if (player->obj.y < enemy->obj.y){
        enemy->obj.dy = -enemy->obj.speed_y *2;
    }
    else{
        enemy->obj.dy =0;
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
int check_if_obj_missed_player_when_moved(GameObject_t obj, Win* win){
    int changey =0;
    int changex =0;
    if(obj.dy >0){
        changey = -1;
    }
    if(obj.dy <0){
        changey = 1;
    }
    if(obj.dx >0){
        changex = -1;
    }
    if(obj.dx <0){
        changex = 1;
    }
    for(int row = obj.dy; row < 0; row+=changey){
        for(int col = obj.dx; col < 0; col+=changex){
            if(win->rows <= obj.y + row  || win->cols <= obj.x + col || obj.y + row <0 || obj.x + col <0){
                continue;
            }
            if(win->map[obj.y + row][obj.x + col] == PLAYER_SPRITE){
                return 0;
            }
        }
    }
    return 0;
}
int check_if_hit_player(GameObject_t obj,Win* win){
    for(int row = 0; row < obj.height; row++){
        for(int col = 0; col < obj.width; col++){
            if(win->rows <= obj.y + row || win->cols <= obj.x + col){
                continue;
            }
            if(win->map[obj.y + row][obj.x + col] == PLAYER_SPRITE){
                return 1;
            }
        }
    }
    return check_if_obj_missed_player_when_moved(obj, win);
}
int check_if_star_hit_player(GameObject_t obj,Win* win){
    for(int row = 0; row < obj.height+2; row++){
        for(int col = 0; col < obj.width+2; col++){
            if(win->rows <= obj.y + row -1 || win->cols <= obj.x + col -1){
                return 0;
            }
            if(win->map[obj.y + row -1][obj.x + col-1] == PLAYER_SPRITE){
                return 1;
            }
        }
    }
    return 0;
}