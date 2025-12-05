#include "game.h"

void create_game_map(Win* win){
    win->map = (char**)malloc(sizeof(char*) * win->rows);
    if (win->map!=NULL){
        for(int y=0;y<win->rows;y++){
            win->map[y]=(char*)malloc(sizeof(char) * win->cols);
            if (win->map[y]==NULL){
                free_map(win->map,y);
                exit(1);
            }
        }
    };
}


void fill_border_to_map( Win* win){
    for (int y=0;y<win->rows;y++){
        for(int x=0;x<win->cols;x++){
            if (x==0 || y==0 || y==win->rows-1 || x==win->cols-1){
                win->map[y][x]=BORDER;
                continue;
            }
            win->map[y][x]=' ';
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
            win->map[obj.y + row][obj.x + col] = ' ';
        }
    }
}
void draw_to_map_obj(GameObject_t obj, Win* win,char symbol){
    for(int row=0; row < obj.height; row++){
        for(int col=0; col < obj.width; col++){
            if(win->rows <= obj.y + row || win->cols <= obj.x + col || obj.y + row <0 || obj.x + col <0){
                return ;
            }
            win->map[obj.y + row][obj.x + col] = symbol;
        }
    }   
}
void free_map(Map_t map, int size) {
    if (map == NULL) return;

    for (int i = 0; i < size; i++) {
        free(map[i]);
    }
    free(map);
}