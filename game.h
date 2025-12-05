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
#define STAR '*'
#define MAX_STARS 1000
#define TAXI_BASE_SPEED 1000

typedef char** Map_t;

typedef char* Sprite_t;

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
    int bounces, damage, alive,sleep_after_dash,dashing,dash_limit;
} Enemy_t;

typedef struct {
    GameObject_t obj;
    int alive;
    int fade_color;
} Star_t;

typedef struct{
    GameObject_t obj;
    int life_force,stars,wanted_x,wanted_y,taxi_cooldown,base_taxi_cooldown;
    char name[PLAYER_NAME_MAX];
}Player_t;

typedef struct {
    WINDOW* window;
    int x, y, rows, cols;
    Map_t map;
} Win;

typedef struct {
    int max_speed,delta_speed,min_speed,map_rows, map_cols,status_rows, status_cols, time_limit_ms, star_quota, player_health, hunter_spawn_rate, hunter_type_count,seed,damage_over_time_mult,score_time_bias,score_star_bias,max_enemys_per_level,star_spawn_chance;
    Player_t* player;
    Enemy_t* hunters;
} LevelConfig_t;

//init.c
void init_ncurses();

//graphic.c
Win* create_window(int rows,int cols,int x, int y,int have_map);
void draw_border(Win* win);
void update_status_display(Win* win, char* name,int level,int time_left,int stars,int stars_quota,int hp);
void mount_upd(Win* win);
void draw_obj(const GameObject_t obj, Win* win);
int display_level_selection_menu();
char* player_name_window();
void congratulate_player_win(char* player_name, int level_num);
void change_sprite_base_on_direction(GameObject_t* obj);
void draw_to_win_and_map(GameObject_t obj, Win* win, char map_char);
void remove_from_win_and_map(GameObject_t obj, Win* win);

//physics.c
int detect_wall_collision(GameObject_t obj, Win* win);
int dash(Enemy_t* enemy,Player_t* player);
int check_if_hit_player(GameObject_t obj,Win* win);
int check_if_star_hit_player(GameObject_t obj,Win* win);

//funcs.c small functions to make code more readable
int calculate_damage(int damage, int time_max, int time_left, int dmg_mul);

//actors.c
void handle_player_input(Player_t* p, char input, Win* win,LevelConfig_t* cfg,int game_speed);
int move_player(Player_t* p, Win* win,LevelConfig_t* cfg);
Player_t* create_player(LevelConfig_t* config);
void destroy_player(Player_t* p);
void setup_player_sprites(Player_t* p, LevelConfig_t* config);
void remove_obj_from_window(GameObject_t obj, Win* win);
Enemy_t* spawn_enemy(Win* win, LevelConfig_t* config,int type,int time_left);
void destroy_enemy_list(Enemy_t** enemy, int count);
void move_enemy(Enemy_t* enemy, Win* win, Player_t* player);
void move_star(Star_t* star, Win* win,Player_t* player);
Star_t* spawn_star(Win* win);
void free_star_list(Star_t** stars, int count);

//map.c
void create_map(Win* win);
void remove_from_map_obj(GameObject_t obj, Win* win);
void draw_to_map_obj(GameObject_t obj, Win* win,char symbol);
void free_map(Map_t map, int size);

//io.c
LevelConfig_t* load_level_config(int level_num) ;
void free_level_config(LevelConfig_t* config);
void load_hunters(char key[64], int value,char* string_val, LevelConfig_t* config, int count);
void load_sprites(char* attribute, char value[256],SpriteList_t* sprite_list);
char* maloc_sprite(LevelConfig_t* config, int i);
void assign_values(LevelConfig_t* config, char* key,char line[256]) ;
void load_player(char key[64],int value, char* string_val, LevelConfig_t* config);
void check_if_sprite_is_correct(GameObject_t* obj);

//game.c
int level_selector(int level_num, char* player_name);
int game_loop(Win* main_win, Win* status_win, Player_t* player, int game_speed, LevelConfig_t* config, Enemy_t** hunters, int* hunters_count, int level_num, Star_t** stars, int* stars_count);

#endif