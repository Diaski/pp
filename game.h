#ifndef GAME_H
#define GAME_H

#include <ncurses.h>

/* ==========================================
   COLORS
   ========================================== */
#define COLOR_PAIR_COUNT 7
#define WHITE_ON_BLACK_PAIR 1
#define RED_ON_BLACK_PAIR 2
#define YELLOW_ON_BLACK_PAIR 3
#define GREEN_ON_BLACK_PAIR 4
#define BLUE_ON_BLACK_PAIR 5
#define CYAN_ON_BLACK_PAIR 6
#define MAGENTA_ON_BLACK_PAIR 7
/* ==========================================
   KEYBINDS
   ==========================================*/
#define K_QUIT 'q'
#define K_TAXI 'e'
#define K_UP 'w'
#define K_DOWN 's'
#define K_LEFT 'a'
#define K_RIGHT 'd'
#define K_SPEED_UP 'p'
#define K_SPEED_DOWN 'o'
#define K_REPLAY 'r'
/* ==========================================
   SYSTEM, TIME & CONFIGURATION
   ========================================== */
#define USC_T_MSC 1000 
#define MAX_FILE_NAME_LENGTH 128
#define MAX_LINE_LENGTH 256
#define MAX_KEY_LENGTH 64
#define MAX_ATTRIBUTE_LENGTH 64
#define MAX_PREFIX_LENGTH 32

/* ==========================================
   ERROR CODES & EXIT STATES
   ========================================== */
#define LOADING_CONFIG_ERROR 202
#define TERMINAL_SIZE_ERROR 303
#define MALLOC_ERROR 404
#define EXIT_GAME 505
#define FILE_ERROR 606
#define REPLAY_GAME 707

/* ==========================================
   GENERAL GAMEPLAY & PHYSICS
   ========================================== */
#define HORIZONTAL 1
#define VERTICAL 2
#define ALIVE 1
#define DEAD 0
#define WINNER 1
#define MAP_EMPTY ' '
#define BORDER 'B'
#define L_MARGIN 1
#define T_MARGIN 1
#define REPLAY_DISABLED 0
#define REPLAY_LOADED 1
#define REPLAY_ENABLED 2
/* ==========================================
   LEVELS & SCORING
   ========================================== */
#define MAX_LEVELS 5
#define DEFAULT_LEVEL 2
#define MIN_LEVEL 1
#define LIFE_FORCE_TO_SCORE_DIVIDER 10

/* ==========================================
   UI: WINDOWS & MENUS
   ========================================== */
#define MIN_WIN_ROWS 10
#define MIN_WIN_COLS 20

// Player Name Input Window
#define GET_PLAYER_NAME_WIN_ROWS 5
#define GET_PLAYER_NAME_WIN_COLS (PLAYER_NAME_MAX + 3)

// Congratulations/Win Window
#define CONGRATULATIONS_WIN_ROWS 20
#define CONGRATULATIONS_WIN_COLS (PLAYER_NAME_MAX + 2) 
#define CONGRATULATIONS_WIN_X 5
#define CONGRATULATIONS_WIN_Y 5
#define CONGRATULATIONS_SLEEP_TIME_USC 2000000

// Level Display Window
#define LEVEL_WIN_ROWS 20
#define LEVEL_WIN_COLS 50
#define LEVEL_WIN_X 5
#define LEVEL_WIN_Y 5

// Leaderboard Window
#define LEADERBOARD_WIN_ROWS 20
#define LEADERBOARD_WIN_COLS 50
#define LEADERBOARD_WIN_X 5
#define LEADERBOARD_WIN_Y 5
#define LEADERBOARD_MAX_PLAYERS 10
#define LEADERBOARD_SLEEP_TIME_USC 5000000

/* ==========================================
   ENTITY: PLAYER
   ========================================== */
#define PLAYER_SPRITE 'P'
#define DEFAULT_PLAYER_NAME "Player1"
#define PLAYER_NAME_MAX 50
#define PLAYER_BASE_SPAWN_X 6
#define PLAYER_BASE_SPAWN_Y 6
#define PLAYER_DIED 100
#define DASH_SPEED_MULTIPLIER 2
#define PLAYER_CHANGE_COLOR_THRESHOLD 2 // when life force is less than half change color

/* ==========================================
   ENTITY: TAXI (PLAYER ABILITY)
   ========================================== */
#define TAXI_SPRITE ".[T]."
#define TAXI_WIDTH 5
#define TAXI_HEIGHT 1
#define TAXI_SPEED 1
#define TAXI_BASE_SPEED 1000
#define TAXI_SAFE_RANGE 1
#define MAX_TAXI_SPAWN_ATTEMPTS 100
#define TAXI_SAFE_MARGIN 2 // makesure taxi does not end too close hunter base of +value in all axis

/* ==========================================
   ENTITY: STARS (COLLECTIBLES)
   ========================================== */
#define STAR '*'
#define STAR_SPRITE '*'
#define MAX_STARS 1000
#define STAR_SCORE 1
#define SPAWN_STAR_Y 1
#define STAR_FAIR_RANGE 1   // 1 means x+1 y+1 and x-1 y-1 area (from 1x1 to 3x3 area)
#define STAR_FADE_CONVERSION_RATE 2 // starts to fade when it goes down by 1/n

/* ==========================================
   ENTITY: ENEMIES (HUNTERS)
   ==========================================*/
#define ENEMY_SPRITE 'E' 
#define BOUNCES_MULT_OVER_TIME 5
#define BOUNCES_CHANGE -1

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
    int max_enemys;
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
void draw_obj(const GameObject_t obj, Win* win);
int display_level_selection_menu(int* replay);
char* get_player_name_window(void);
void congratulate_player_win(char* player_name, int level_num,int res);
void change_sprite_base_on_direction(GameObject_t* obj);
void draw_to_win_and_map(GameObject_t obj, Win* win, char map_char);
void remove_from_win_and_map(GameObject_t obj, Win* win);
void del_window(Win* win);
void change_blink_sprite_base_on_direction(Player_t* p);
void remove_obj_from_window(GameObject_t obj, Win* win);

//physics.c
int detect_wall_collision(GameObject_t obj, Win* win);
int check_if_hit_player(GameObject_t obj,Win* win);
int check_if_star_hit_player(GameObject_t obj,Win* win);
int check_if_missed_player(GameObject_t* player, GameObject_t* enemy);
int reverse_when_touch_border(GameObject_t* obj, Win* win);

//map.c
void create_map(Win* win);
void remove_from_map_obj(GameObject_t obj, Win* win);
void draw_to_map_obj(GameObject_t obj, Win* win,char symbol);
void free_map(Map_t map, int size);
int find_safe_spot(Win* win, int x, int y, Player_t* p);

//io.c
LevelConfig_t* load_level_config(int level_num);
void assign_values(LevelConfig_t* config, char* key,char* line);
void base_asignements(LevelConfig_t* c, char* k,int v);
void load_hunters(char* k, int v,char* string_val, LevelConfig_t* cfg, int count);
void load_player(char* k,int v, char* string_val, LevelConfig_t* c);
void load_sprites(char* attr, char* value,SpriteList_t* sprite_list);
void load_blink_sprites(char* attr, char* value,SpriteList_t* sprite_list);
void check_if_sprite_loaded_correct(GameObject_t* obj);
void check_if_blink_loaded_correct(Player_t* p);
int check_config(LevelConfig_t* config);
void free_level_config(LevelConfig_t* config);

//leaderboard.c
LeaderboardEntry_t* load_leaderboard_entries(int entry_count);
int save_to_leaderboard(char* player_name, int score);
void free_leaderboard_entries(LeaderboardEntry_t* entries, int entry_count);
int count_leaderboard_entries(void);
void show_leaderboard(void);

//game.c
int validate_input(char input, Player_t* player, int* game_speed, LevelConfig_t* config, int time_left);
void end_of_tick(int game_speed,int* time_left);
void free_all_from_level(LevelConfig_t* config, Enemy_t** hunters, int hunters_count, Star_t** stars, int stars_count,
   Win* main_win, Win* status_win, Player_t* player);
void end_level(char* player_name, int level_num, int res, int score,int* replay);
int level_selector(char* p_name,int* replay,char** save);
int game_loop(Win* main_win, Win* status_win, Player_t* p, int g_speed,LevelConfig_t* cfg,
   Enemy_t** hunters,int* hunters_c,int lvl_num, Star_t** stars,int* stars_c,int* time_left,char** save, int* replay);

//star.c
void random_star_spawn(Win* win, LevelConfig_t* config, Star_t** stars, int* stars_count);
void move_star_list(Star_t** stars, int stars_count, Win* win,Player_t* player,int only_draw);
void move_star(Star_t* star, Win* win,Player_t* player,int only_draw);
Star_t* spawn_star(Win* win);
void free_star_list(Star_t** stars, int count);

//enemy.c
Enemy_t* spawn_enemy(Win* win, LevelConfig_t* config,int type,int time_left);
void setup_enemy_data_base_on_template(Enemy_t* enemy, Enemy_t temp,int time_left, LevelConfig_t* config);
void setup_enemy_in_window(Enemy_t* enemy, Win* win);
void move_enemy(Enemy_t* enemy, Win* win, Player_t* player,int only_draw);
void random_enemy_spawn(Win* win, LevelConfig_t* config, Enemy_t** hunters, int* hunters_c,int time_left);
void move_enemy_list(Enemy_t*** hunters, int hunters_count, Win* win, Player_t* player,int only_draw);
int dash(Enemy_t* enemy,Player_t* player);
int calculate_damage(int damage, int time_max, int time_left, int dmg_mul);
int calculate_bounces(int bounces,int time_max,int time_left);
void destroy_enemy_list(Enemy_t** enemy, int count);

//player.c
Player_t* create_player(LevelConfig_t* config);
int move_player(Player_t* p, Win* win,int only_draw);
void setup_player_sprites(Player_t* p, LevelConfig_t* config);
void handle_player_input(Player_t* p, char input, Win* win,LevelConfig_t* cfg,int game_speed,
    Enemy_t** hunters,int hunters_count, Star_t** stars,int stars_count);
void taxi(Player_t* p, Win* win,int game_speed, Enemy_t** hunters,int hunters_count, Star_t** stars,
   int stars_count);void destroy_player(Player_t* p);
int calculate_score(LevelConfig_t* level_config,Player_t* player,int time_max, int time_left);
void create_save(char** save,LevelConfig_t* cfg);

#endif