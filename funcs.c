#include "game.h"
void free_map(Map_t map,int size){
    for(int x=0;x<size;x++){
        free(map[x]);
    }
    free(map);
}
void copy_sprite(Sprite_t dest, const Sprite_t src){
    strcpy(dest,src);
}