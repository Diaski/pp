#include "game.h"
#include <stdlib.h>

Map_t create_game_map(int rows,int cols){
    Map_t map = malloc(sizeof(char*)*rows);
    if (map!=NULL){
        for(int x=0;x<rows;x++){
            map[x]=malloc(sizeof(char)*cols);
            if (map[x]==NULL){
                free_map(map,x);
                return NULL;
            }
        }
    }
    return map;
}


void fill_border_to_map(Map_t map,int rows,int cols){
    for (int y=0;y<rows;y++){
        for(int x=0;x<cols;x++){
            if (x==0 || y==0 || y==rows-1 || x==cols-1){
                map[y][x]=BORDER;
                continue;
            }
            map[y][x]=' ';
        }
    }
}

Map_t create_map(int rows,int cols){
    Map_t map = create_game_map(rows,cols);
    fill_border_to_map(map,rows,cols);
    return map;
}
int detect_wall_collision(GameObject_t obj, Map_t map){
    for(int i=0; i<obj.dx;i++){
        if(map[obj.y][obj.x +obj.dx]==BORDER){
            return HORIZONTAL;
        }
    }
    for(int i=0;i<obj.dy;i++){
        if(map[obj.y+obj.dy][obj.x]==BORDER){
            return VERTICAL;
        }
    }
    return 0;
}