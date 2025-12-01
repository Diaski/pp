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