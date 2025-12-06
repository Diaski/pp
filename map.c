#include "game.h"
#include <stdlib.h>

void create_game_map(Win* win){
    win->map = (char**)calloc((unsigned int)win->rows, sizeof(char*));
    if (win->map!=NULL){
        for(int y=0;y<win->rows;y++){
            win->map[y]=(char*)calloc((unsigned int)win->cols, sizeof(char));
            if (win->map[y]==NULL){
                free_map(win->map,y);
                exit(MALLOC_ERROR);
            }
        }
    }
    else{
        exit(MALLOC_ERROR);
    }
}

void fill_border_to_map( Win* win){
    for (int y=0;y<win->rows;y++){
        for(int x=0;x<win->cols;x++){
            if (x==0 || y==0 || y==win->rows-1 || x==win->cols-1){
                win->map[y][x]=BORDER;
                continue;
            }
            win->map[y][x]=MAP_EMPTY;
        }
    }
}

void create_map(Win* win){
    create_game_map(win);
    fill_border_to_map(win);
}   

void remove_from_map_obj(GameObject_t obj, Win* win){
    for(int row=0; row < obj.height; row++){
        for(int col=0; col < obj.width; col++){
            win->map[obj.y + row][obj.x + col] = MAP_EMPTY;
        }
    }
}

void draw_to_map_obj(GameObject_t obj, Win* win,char symbol){
    for(int row=0; row < obj.height; row++){
        for(int col=0; col < obj.width; col++){
            if(win->rows <= obj.y + row || win->cols <= obj.x + col || obj.y + row <0 || obj.x + col <0){
                continue;
            }
            win->map[obj.y + row][obj.x + col] = symbol;
        }
    }   
}

int detect_if_spot_hunter(Win* win, int x, int y,int width, int height){
    for(int row=-height;row<height;row++){
        for(int col=-width;col<width;col++){
            if(x+col<0 || y+row<0 || x+col>=win->cols || y+row>=win->rows){
                return true;
            }
            if(win->map[y+row][x+col]==ENEMY_SPRITE){
                return true;
            }
        }
    }
    return false;
}

void free_map(Map_t map, int size) {
    if (map == NULL) return;

    for (int i = 0; i < size; i++) {
        free((void*)map[i]);
    }
    free((void*)map);
}