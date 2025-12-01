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
Player_t* create_player(LevelConfig_t* config){
    Player_t* p = malloc(sizeof(Player_t));
    p->obj.width=config->player->obj.width;
    p->obj.height=config->player->obj.height;
    p->obj.x = 10;
    p->obj.y = 10;
    p->obj.speed_x = config->player->obj.speed_x;
    p->obj.speed_y = config->player->obj.speed_y;
    p->obj.sprites_list = config->player->obj.sprites_list;
    p->obj.current_sprite = p->obj.sprites_list.down;
    p->life_force = config->player->life_force;
    return p;
}
void destroy_player(Player_t* p){
    free(p);
}