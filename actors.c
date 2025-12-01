#include "game.h"

int move_player(Player_t* p, Win* win){
    if(detect_wall_collision(p->obj, win)!=0){return 0;};
    remove_from_map_obj(p->obj, win);
    p->obj.x += p->obj.dx;
    p->obj.y += p->obj.dy;
    if(p->obj.dx > 0){
        p->obj.current_sprite = p->obj.sprites_list.right;
        draw_to_map_obj(p->obj,win, PLAYER_SPRITE);
        return 0;
    } else if(p->obj.dx < 0){
        p->obj.current_sprite = p->obj.sprites_list.left;
        draw_to_map_obj(p->obj,win, PLAYER_SPRITE);
        return 0;
    } else if(p->obj.dy > 0){
        p->obj.current_sprite = p->obj.sprites_list.down;
        draw_to_map_obj(p->obj,win, PLAYER_SPRITE);
        return 0;
    } else if(p->obj.dy < 0){
        p->obj.current_sprite = p->obj.sprites_list.up;
        draw_to_map_obj(p->obj,win, PLAYER_SPRITE);
        return 0;
    }
    return 1;
}
void handle_player_input(Player_t* p, char input, Win* win){
    switch(input){
        case 'w':
            p->obj.dx = 0;
            p->obj.dy = -p->obj.speed_y;
            move_player(p, win);
            break;
        case 's':
            p->obj.dx = 0;
            p->obj.dy = p->obj.speed_y;
            move_player(p, win);
            break;
        case 'a':
            p->obj.dx = -p->obj.speed_x;
            p->obj.dy = 0;
            move_player(p, win);
            break;
        case 'd':
            p->obj.dx = p->obj.speed_x;
            p->obj.dy = 0;
            move_player(p, win);
            break;
        default:
            break;
    }
}
Player_t* create_player(){
    Player_t* p = malloc(sizeof(Player_t));
    p->obj.width=3;
    p->obj.height=3;
    p->obj.x = 10;
    p->obj.y = 10;
    p->obj.speed_x = 1;
    p->obj.speed_y = 1;
    setup_player_sprites(p);
    p->obj.current_sprite = p->obj.sprites_list.down;
    p->life_force = 100;
    return p;
}
void setup_player_sprites(Player_t* p){
    p->obj.sprites_list.down = malloc(sizeof(char)*p->obj.width*p->obj.height +1);
    p->obj.sprites_list.up = malloc(sizeof(char)*p->obj.width*p->obj.height +1);
    p->obj.sprites_list.left = malloc(sizeof(char)*p->obj.width*p->obj.height +1);
    p->obj.sprites_list.right = malloc(sizeof(char)*p->obj.width*p->obj.height +1);
    if(!p->obj.sprites_list.down || !p->obj.sprites_list.up || !p->obj.sprites_list.left || !p->obj.sprites_list.right){
        exit(1);
    }
    strcpy(p->obj.sprites_list.down, ".|./|\\.v.");
    strcpy(p->obj.sprites_list.up, ".^./|\\.|.");
    strcpy(p->obj.sprites_list.left, "./.<--.\\.");
    strcpy(p->obj.sprites_list.right, ".\\.-->./.");
}
void destroy_player(Player_t* p){
    free(p->obj.sprites_list.down);
    free(p->obj.sprites_list.up);
    free(p->obj.sprites_list.left);
    free(p->obj.sprites_list.right);
    free(p);
}