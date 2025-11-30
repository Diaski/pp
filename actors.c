#include "game.h"

void setup_player_sprites(Player_t* p){
    p->obj.sprites_list.down = malloc(sizeof(char)*p->obj.width*p->obj.height);
    p->obj.sprites_list.up = malloc(sizeof(char)*p->obj.width*p->obj.height);
    p->obj.sprites_list.left = malloc(sizeof(char)*p->obj.width*p->obj.height);
    p->obj.sprites_list.right = malloc(sizeof(char)*p->obj.width*p->obj.height);
    if(!p->obj.sprites_list.down || !p->obj.sprites_list.up || !p->obj.sprites_list.left || !p->obj.sprites_list.right){
        exit(1);
    }
    p->obj.sprites_list.down = ".^./|\\.|.";
    p->obj.sprites_list.up = ". .\\|/..";
    p->obj.sprites_list.left = "./..<<...";
    p->obj.sprites_list.right = "...>>..\\.";
}
Player_t* create_player(){
    Player_t* p = malloc(sizeof(Player_t));
    p->obj.width=3;
    p->obj.height=3;
    p->obj.x = 10;
    p->obj.y = 10;
    setup_player_sprites(p);
    p->obj.current_sprite = p->obj.sprites_list.down;
    p->life_force = 100;
    return p;
}
void destroy_player(Player_t* p){
    free(p->obj.sprites_list.down);
    free(p->obj.sprites_list.up);
    free(p->obj.sprites_list.left);
    free(p->obj.sprites_list.right);
    free(p);
}
void move_player(Player_t* p, int dx, int dy){
    p->obj.x += dx;
    p->obj.y += dy;
    if(dx > 0){
        p->obj.current_sprite = p->obj.sprites_list.right;
    } else if(dx < 0){
        p->obj.current_sprite = p->obj.sprites_list.left;
    } else if(dy > 0){
        p->obj.current_sprite = p->obj.sprites_list.down;
    } else if(dy < 0){
        p->obj.current_sprite = p->obj.sprites_list.up;
    }
}
void draw_player(const Player_t* p,Win* win){
    for(int row = 0; row < p->obj.height; row++){
        for(int col = 0; col < p->obj.width; col++){
            mvwaddch(win->window, p->obj.y + row, p->obj.x + col, p->obj.current_sprite[row * p->obj.width + col]);
        }
    }
}
void handle_player_input(Player_t* p, char input){
    switch(input){
        case 'w':
            move_player(p, 0, -1);
            break;
        case 's':
            move_player(p, 0, 1);
            break;
        case 'a':
            move_player(p, -1, 0);
            break;
        case 'd':
            move_player(p, 1, 0);
            break;
        default:
            break;
    }
}