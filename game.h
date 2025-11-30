#ifndef GAME_H
#define GAME_H

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

#define WHITE_ON_BLACK_PAIR 1
#define RED_ON_BLACK_PAIR 2
#define YELLOW_ON_BLACK_PAIR 3
#define HORIZONTAL 1
#define VERTICAL 2
#define BORDER 'B'


typedef char** Map_t;

typedef char* Sprite_t;

typedef struct {
    Sprite_t left;
    Sprite_t right;
    Sprite_t up;
    Sprite_t down;
}Sprite_List_t;

typedef struct {
    int x,y,dx,dy,width,height;
    Sprite_List_t sprites_list;
    Sprite_t current_sprite;
}GameObject_t;

typedef struct{
    GameObject_t obj;
    int life_force;
}Player_t;

typedef struct {
    WINDOW* window;
    int x, y, rows, cols;
    Map_t map;
} Win;



typedef struct{
    int map_cols,map_rows;
}Level_Config_t;

typedef struct{
    Win* main_win,status_win;
}Config_t;

//init.c
void init_ncurses();

//graphic.c
Win* create_window(int rows, int cols, int y, int x);
void draw_border(Win* win);
void update_status_display(Win* win, int time, int level);
void mount_upd(Win* win);

//physics.c
Map_t create_map(int rows,int cols);
int detect_wall_collision(GameObject_t obj, Map_t map);

//funcs.c small functions to make code more readable
void free_map(Map_t map,int size);
void copy_sprite(Sprite_t dest, const Sprite_t src);

//actors.c
void handle_player_input(Player_t* p, char input);
void draw_player(const Player_t* p,Win* win);
void move_player(Player_t* p, int dx, int dy);
Player_t* create_player();
void destroy_player(Player_t* p);
void setup_player_sprites(Player_t* p);


#endif