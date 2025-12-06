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
    return false;
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
                return true;
            }
        }
    }
    return false;
}

int check_if_hit_player(GameObject_t obj,Win* win){
    for(int row = 0; row < obj.height; row++){
        for(int col = 0; col < obj.width; col++){
            if(win->rows <= obj.y + row || win->cols <= obj.x + col){
                continue;
            }
            if(win->map[obj.y + row][obj.x + col] == PLAYER_SPRITE){
                return true;
            }
        }
    }
    return check_if_obj_missed_player_when_moved(obj, win);
}

int check_if_star_hit_player(GameObject_t obj,Win* win){
    for(int row = 0; row < obj.height+(STAR_FAIR_RANGE*2); row++){
        for(int col = 0; col < obj.width+(STAR_FAIR_RANGE*2); col++){
            const int check_row = row - STAR_FAIR_RANGE;
            const int check_col = col - STAR_FAIR_RANGE;
            
            if (check_row < 0 || check_row >= win->rows || check_col < 0 || check_col >= win->cols) {
                continue;
            }
            
            if(win->map[obj.y + row -STAR_FAIR_RANGE][obj.x + col-STAR_FAIR_RANGE] == PLAYER_SPRITE){
                return true;
            }
        }
    }
    return false;
}

int check_if_missed_player(GameObject_t* player, GameObject_t* enemy){
    if(enemy->dx >0){
        if(enemy->x >= player->x){
            return true;
        }
    }
    else if (enemy->dx <0){
        if(enemy->x <= player->x){
            return true;
        }
    }
    if(enemy->dy >0){
        if(enemy->y + enemy->height >= player->y){
            return true;
        }
    }
    else if (enemy->dy <0){
        if(enemy->y <= player->y + player->height){
            return true;
        }
    }
    return false;
}

int reverse_when_touch_border(GameObject_t* obj, Win* win){
    int jumped=0;
    int collision = detect_wall_collision(*obj, win);
    if(collision == HORIZONTAL){
        obj->dx = -obj->dx;
        jumped=true;
    }
    collision = detect_wall_collision(*obj, win);
    if(collision == VERTICAL){
        obj->dy = -obj->dy;
        jumped=true;
    }
    return jumped;
}