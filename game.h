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
#define GREEN_ON_BLACK_PAIR 4
#define BLUE_ON_BLACK_PAIR 5
#define CYAN_ON_BLACK_PAIR 6
#define MAGENTA_ON_BLACK_PAIR 7
#define COLOR_PAIR_COUNT 7
#define HORIZONTAL 1
#define VERTICAL 2
#define BORDER 'B'
#define PLAYER_SPRITE 'P'
#define ENEMY_SPRITE 'E'
#define STAR_SPRITE '*'
#define PLAYER_NAME_MAX 100

typedef char** Map_t;

typedef char* Sprite_t;

typedef enum Colors{
    white = WHITE_ON_BLACK_PAIR,
    red = RED_ON_BLACK_PAIR,
    yellow = YELLOW_ON_BLACK_PAIR,
    green = GREEN_ON_BLACK_PAIR,
    blue = BLUE_ON_BLACK_PAIR,
    cyan = CYAN_ON_BLACK_PAIR,
    meganta = MAGENTA_ON_BLACK_PAIR
}Colors_t;

typedef struct {
    Sprite_t left;
    Sprite_t right;
    Sprite_t up;
    Sprite_t down;
}SpriteList_t;

typedef struct {
    int x,y,dx,dy,width,height;
    int speed_x;int speed_y;
    int color;
    SpriteList_t sprites_list;
    Sprite_t current_sprite;
}GameObject_t;

typedef struct {
    GameObject_t obj;
    int bounces, damage, alive;
} Enemy_t;

typedef struct {
    GameObject_t obj;
    int alive;
} Star_t;

typedef struct{
    GameObject_t obj;
    int life_force,score;
    char name[PLAYER_NAME_MAX];
}Player_t;

typedef struct {
    WINDOW* window;
    int x, y, rows, cols;
    Map_t map;
} Win;


typedef struct {
    int max_speed,delta_speed,min_speed,map_rows, map_cols,status_rows, status_cols, time_limit_ms, star_quota, player_health, hunter_spawn_rate, hunter_type_count,seed,damage_over_time_mult,score_time_bias,score_star_bias,max_enemys_per_level;
    Player_t* player;
    Enemy_t* hunters;
} LevelConfig_t;

//init.c
void init_ncurses();

//graphic.c
Win* create_window(int rows,int cols,int x, int y,int have_map);
void draw_border(Win* win);
void update_status_display(Win* win, int x, int y,int gamespeed,char input,int hp);
void mount_upd(Win* win);
void draw_obj(const GameObject_t obj, Win* win);

//physics.c
int detect_wall_collision(GameObject_t obj, Win* win);

//funcs.c small functions to make code more readable
void copy_sprite(Sprite_t dest, const Sprite_t src);
void calculate_damage(Enemy_t* enemy, int time_max, int time_left, int dmg_mult);

//actors.c
void handle_player_input(Player_t* p, char input, Win* win);
int move_player(Player_t* p, Win* win);
Player_t* create_player(LevelConfig_t* config);
void destroy_player(Player_t* p);
void setup_player_sprites(Player_t* p, LevelConfig_t* config);
void remove_obj_from_window(GameObject_t obj, Win* win);
Enemy_t* spawn_enemy(Win* win, LevelConfig_t* config,int type);
void destroy_enemy_list(Enemy_t** enemy, int count);
void move_enemy(Enemy_t* enemy, Win* win, Player_t* player);

//map.c
void create_map(Win* win);
void remove_from_map_obj(GameObject_t obj, Win* win);
void draw_to_map_obj(GameObject_t obj, Win* win,char symbol);
void free_map(Map_t map, int size);

//io.c
LevelConfig_t* load_level_config(int level_num) ;
void free_level_config(LevelConfig_t* config);

#endif