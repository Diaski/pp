#ifndef GAME_H
#define GAME_H

#include <ncurses.h>

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
#define PLAYER_NAME_MAX 50
#define STAR '*'
#define MAX_STARS 1000
#define TAXI_BASE_SPEED 1000
#define PLAYER_DIED 100
#define WINNER 1
#define BOUNCES_MULT_OVER_TIME 3
#define STAR_FAIR_RANGE 1 // 1 means  x+1 y+1 and x-1 y-1 area so 1x1 becomes 3x3 area
#define DASH_SPEED_MULTIPLIER 2
#define STAR_FADE_CONVERSION_RATE 2 //means when it goes down by 1/n where n is this value its start to fade
#define MAX_FILE_NAME_LENGTH 128
#define MAX_LINE_LENGTH 256
#define MAX_KEY_LENGTH 64
#define MAX_ATTRIBUTE_LENGTH 64
#define MAX_PREFIX_LENGTH 32
#define PLAYER_BASE_SPAWN_X 6
#define PLAYER_BASE_SPAWN_Y 6
#define USC_T_MSC 1000 
#define CONGRATULATIONS_SLEEP_TIME_USC 2000000
#define CONGRATULATIONS_WIN_ROWS 20
#define CONGRATULATIONS_WIN_COLS (PLAYER_NAME_MAX + 2) //prefered to be PLAYER_NAME_MAX + 2 can be changed but display doesnt support it well
#define CONGRATULATIONS_WIN_X 5
#define CONGRATULATIONS_WIN_Y 5
#define LEVEL_WIN_ROWS 20
#define LEVEL_WIN_COLS 50
#define LEVEL_WIN_X 5
#define LEVEL_WIN_Y 5
#define MAX_LEVELS 5
#define LOADING_CONFIG_ERROR 2
#define TERMINAL_SIZE_ERROR 3
#define MALLOC_ERROR 4
#define EXIT_GAME 5
#define LEADERBOARD_WIN_ROWS 20
#define LEADERBOARD_WIN_COLS 50
#define LEADERBOARD_WIN_X 5
#define LEADERBOARD_WIN_Y 5
#define LEADERBOARD_MAX_PLAYERS 10
#define LEADERBOARD_SLEEP_TIME_USC 5000000
#define LIFE_FORCE_TO_SCORE_DIVIDER 10
#define TAXI_WIDTH 5
#define TAXI_HEIGHT 1
#define TAXI_SPEED 1
#define TAXI_SPRITE ".[T]."


typedef char** Map_t;

typedef char* Sprite_t;

typedef struct {
    char* name;
    int score;
} LeaderboardEntry_t;

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
    int bounces, damage, alive,sleep_after_dash,dashing,dash_limit,base_dash_sleep_time;
} Enemy_t;

typedef struct {
    GameObject_t obj;
    int alive;
    int fade_color;
} Star_t;

typedef struct{
    GameObject_t obj;
    SpriteList_t blink_sprites;
    int life_force,stars,taxi_cooldown,base_taxi_cooldown,blink_tick,blink_tick_ratio;
    char name[PLAYER_NAME_MAX];
}Player_t;

typedef struct {
    WINDOW* window;
    int x, y, rows, cols;
    Map_t map;
} Win;

typedef struct {
    int max_speed;
    int delta_speed;
    int min_speed;
    int map_rows;
    int map_cols;
    int status_rows;
    int status_cols;
    int time_limit_ms;
    int star_quota;
    int base_score;
    int player_health;
    int hunter_spawn_rate;
    int hunter_type_count;
    int seed;
    int damage_over_time_mult;
    int score_time_bias;
    int score_star_bias;
    int max_enemys_per_level;
    int star_spawn_chance;
    Player_t* player;
    Enemy_t* hunters;
} LevelConfig_t;

//init.c
void init_ncurses(void);

//graphic.c
Win* create_window(int rows,int cols,int x, int y,int have_map);
void draw_border(Win* win);
void update_status_display(Win* win, char* name,int level,int time_left,int stars,int stars_quota,int hp);
void mount_upd(Win* win);
void draw_obj(const GameObject_t obj, Win* win);
int display_level_selection_menu(void);
char* player_name_window(void);
void congratulate_player_win(char* player_name, int level_num);
void change_sprite_base_on_direction(GameObject_t* obj);
void draw_to_win_and_map(GameObject_t obj, Win* win, char map_char);
void remove_from_win_and_map(GameObject_t obj, Win* win);
void del_window(Win* win);
void show_leaderboard(void);
void change_blink_sprite_base_on_direction(Player_t* p);

//physics.c
int detect_wall_collision(GameObject_t obj, Win* win);
int dash(Enemy_t* enemy,Player_t* player);
int check_if_hit_player(GameObject_t obj,Win* win);
int check_if_star_hit_player(GameObject_t obj,Win* win);
int check_if_missed_player(GameObject_t* player, GameObject_t* enemy);

//funcs.c small functions to make code more readable
int calculate_damage(int damage, int time_max, int time_left, int dmg_mul);
int calculate_bounces(int bounces,int time_max,int time_left);
int calculate_score(LevelConfig_t* level_config,Player_t* player,int time_max, int time_left);

//actors.c
void handle_player_input(Player_t* p, char input, Win* win,LevelConfig_t* cfg,int game_speed, Enemy_t** hunters,int hunters_count, Star_t** stars,int stars_count);
int move_player(Player_t* p, Win* win,int only_draw);
Player_t* create_player(LevelConfig_t* config);
void destroy_player(Player_t* p);
void setup_player_sprites(Player_t* p, LevelConfig_t* config);
void remove_obj_from_window(GameObject_t obj, Win* win);
Enemy_t* spawn_enemy(Win* win, LevelConfig_t* config,int type,int time_left);
void destroy_enemy_list(Enemy_t** enemy, int count);
void move_enemy(Enemy_t* enemy, Win* win, Player_t* player,int only_draw);
void move_star(Star_t* star, Win* win,Player_t* player,int only_draw);
Star_t* spawn_star(Win* win);
void free_star_list(Star_t** stars, int count);
void taxi(Player_t* p, Win* win,int game_speed, Enemy_t** hunters,int hunters_count, Star_t** stars,int stars_count);
int reverse_when_touch_border(GameObject_t* obj, Win* win);

//map.c
void create_map(Win* win);
void remove_from_map_obj(GameObject_t obj, Win* win);
void draw_to_map_obj(GameObject_t obj, Win* win,char symbol);
void free_map(Map_t map, int size);
int detect_if_spot_hunter(Win* win, int x, int y,int width, int height);

//io.c
LevelConfig_t* load_level_config(int level_num) ;
void free_level_config(LevelConfig_t* config);
void load_hunters(char* key, int value,char* string_val, LevelConfig_t* config, int count);
void load_sprites(char* attr, char* value,SpriteList_t* sprite_list);
char* maloc_sprite(LevelConfig_t* config, int i);
void assign_values(LevelConfig_t* config, char* key,char* line) ;
void load_player(char* key,int value, char* string_val, LevelConfig_t* config);
void check_if_sprite_is_correct(GameObject_t* obj);
int save_to_leaderboard(char* player_name, int score);
LeaderboardEntry_t* load_leaderboard_entries(int entry_count);

//game.c
int level_selector(char* player_name);
void move_star_list(Star_t** stars, int count, Win* win, Player_t* player,int only_draw);
void move_enemy_list(Enemy_t*** enemy, int count, Win* win, Player_t* player,int only_draw);
int game_loop(Win* main_win, Win* status_win, Player_t* player, int game_speed, LevelConfig_t* config, Enemy_t** hunters, int* hunters_count, int level_num, Star_t** stars, int* stars_count);
#endif