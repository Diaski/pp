#include "game.h"
#include <stdlib.h>

void move_star_list(Star_t** stars, int count, Win* win, Player_t* player,int only_draw){
    for (int i = 0; i < count; i++) {
        if(stars[i]->alive){
            move_star(stars[i], win, player, only_draw);
        }
    }
}
void random_star_spawn(Win* win, LevelConfig_t* config, Star_t** stars, int* stars_count){
    if(rand()%config->star_spawn_chance==0 && *stars_count < MAX_STARS){
        Star_t* new_star=spawn_star(win);
        if(new_star != NULL){
            stars[*stars_count]=new_star;
            (*stars_count)++;
        }
    }

}

void setup_star(Star_t* star, Win* win){
    star->obj.width = 1;
    star->obj.height = 1;
    star->obj.x = rand() % (win->cols - 2) + 1;
    star->obj.y = SPAWN_STAR_Y;
    star->obj.dx = 0;
    star->obj.dy = 1;
    star->obj.color = WHITE_ON_BLACK_PAIR;
    star->alive = true;
    star->obj.current_sprite = (Sprite_t)calloc(2,sizeof(char));
    star->obj.current_sprite[0] = STAR;
    star->obj.current_sprite[1] = '\0';
    star->fade_color = YELLOW_ON_BLACK_PAIR;
}

Star_t* spawn_star(Win* win){
    Star_t* star = (Star_t*)calloc(1,sizeof(Star_t));
    setup_star(star, win);
    return star;
}

void check_star_hit(Star_t* star, Win* win,Player_t* player){
    if(check_if_hit_player(star->obj, win)){
        star->alive = false;
        player->stars += STAR_SCORE;
        return ;
    }
    if(win->map[star->obj.y][star->obj.x] == ENEMY_SPRITE){
        star->alive = false;
    }
    if(star->obj.y >= win->rows -1){
        star->alive = false;
        return;
    }
}

void move_star(Star_t* star, Win* win,Player_t* player,int only_draw){
    if(only_draw){
        draw_to_win_and_map(star->obj, win, STAR);
        return;
    }
    check_star_hit(star, win, player);
    if(star->alive == false){
        return;
    }
    remove_from_win_and_map(star->obj, win);
    if(check_if_star_hit_player(star->obj, win)){
        star->alive = false;
        player->stars += STAR_SCORE;
    }
    if(star->alive == false){
        return;
    }
    star->obj.y += star->obj.dy;
    if (star->obj.y >= (win->rows/STAR_FADE_CONVERSION_RATE)) {
        star->obj.color = star->fade_color;
    }
    check_star_hit(star, win, player);
    if(star->alive ==false){
        return;
    }
    draw_to_win_and_map(star->obj, win, STAR);
}

void free_star_list(Star_t** stars, int count){
    for (int i = 0; i < count; i++) {
        free((void*)stars[i]->obj.current_sprite);
        free(stars[i]);
    }
    free((void*)stars);
}