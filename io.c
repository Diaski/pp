#include "game.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void load_blink_sprites(char* attr, char* value,SpriteList_t* sprite_list);
void check_if_blink_loaded_correct(Player_t* p);

LevelConfig_t* load_level_config(int level_num) {
    LevelConfig_t* config = (LevelConfig_t*)calloc(1,sizeof(LevelConfig_t));
    config->player = NULL;
    config->hunters = NULL;
    char filename[MAX_FILE_NAME_LENGTH];
    snprintf(filename, sizeof(filename), "data/level%d.conf", level_num);
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        exit(1);
    }
    char line[MAX_LINE_LENGTH], key[MAX_KEY_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        assign_values(config, key,line);
    }
    fclose(file);
    check_if_sprite_is_correct(&config->player->obj);
    check_if_blink_loaded_correct(config->player);
    for(int i = 0; i < config->hunter_type_count; i++) {
        check_if_sprite_is_correct(&config->hunters[i].obj);
    }

    return config;  
}
void base_asignements(LevelConfig_t* c, char* k,int v) {
    if (!strcmp(k, "map_rows")) c->map_rows = v;
    if (!strcmp(k, "map_cols")) c->map_cols = v;
    if (!strcmp(k, "status_rows")) c->status_rows = v;
    if (!strcmp(k, "status_cols")) c->status_cols = v;
    if (!strcmp(k, "time_limit_ms")) c->time_limit_ms = v;
    if (!strcmp(k, "star_quota")) c->star_quota = v;
    if (!strcmp(k, "hunter_spawn_rate")) c->hunter_spawn_rate = v;
    if (!strcmp(k, "max_speed")) c->max_speed = v;
    if (!strcmp(k, "delta_speed")) c->delta_speed = v;
    if (!strcmp(k, "min_speed")) c->min_speed = v;
    if (!strcmp(k, "damage_over_time_mult")) c->damage_over_time_mult = v;
    if (!strcmp(k, "score_time_bias")) c->score_time_bias = v;
    if (!strcmp(k, "score_star_bias")) c->score_star_bias = v;
    if (!strcmp(k, "seed")) c->seed = v;
    if (!strcmp(k, "max_enemys_per_level")) c->max_enemys_per_level = v;
    if (!strcmp(k, "star_spawn_chance")) c->star_spawn_chance = v;
    if (!strcmp(k, "base_score")) c->base_score = v;
}

void assign_values(LevelConfig_t* config, char* key,char* line) {
        int value;
        char attr[MAX_ATTRIBUTE_LENGTH];
        if (sscanf(line, "%[^=]=%d", key, &value)== 2) {
            base_asignements( config, key, value);
            if (strcmp(key, "hunter_type_count") == 0) {
                config->hunter_type_count = value;
                config->hunters = (Enemy_t*)calloc((unsigned int)value, sizeof(Enemy_t));
                if(config->hunters == NULL) {
                    exit(1);
                }
            }
            load_hunters(key,value,"", config, config->hunter_type_count);
            load_player(key, value, "", config);
        }
        else if(sscanf(line, "%[^=]=%s", key, attr) == 2) {
            if(config->hunter_type_count ) {
                load_hunters(key,0,attr, config, config->hunter_type_count);
                load_player(key, 0, attr, config);
            }
        }
}

void load_hunters(char* k, int v,char* string_val, LevelConfig_t* cfg, int count) {
    for(int i=0; i < count; i++) {
        char prefix[MAX_PREFIX_LENGTH];
        snprintf(prefix, sizeof(prefix), "hunter_%d_", i+1);
        if (strncmp(k, prefix, strlen(prefix)) != 0) continue;
        char attr[MAX_ATTRIBUTE_LENGTH];
        sscanf(k + strlen(prefix), "%[^=]", attr);
        if (!strcmp(attr, "width")) cfg->hunters[i].obj.width = v;
        else if (!strcmp(attr, "height")) cfg->hunters[i].obj.height = v;
        else if (!strcmp(attr, "speed_x")) cfg->hunters[i].obj.speed_x = v;
        else if (!strcmp(attr, "speed_y")) cfg->hunters[i].obj.speed_y = v;
        else if (!strcmp(attr, "bounces")) cfg->hunters[i].bounces = v;
        else if (!strcmp(attr, "damage")) cfg->hunters[i].damage = v;
        else if (!strcmp(attr, "color")) cfg->hunters[i].obj.color = v;
        else if (!strcmp(attr, "dash_limit")) cfg->hunters[i].dash_limit = v;
        else if (!strcmp(attr, "base_dash_sleep_time")) cfg->hunters[i].base_dash_sleep_time = v;
        else {
            load_sprites(attr, string_val,&cfg->hunters[i].obj.sprites_list);
        }
    }
}

void load_sprites(char* attr, char* value,SpriteList_t* sprite_list) {
    if (strcmp(attr, "left") == 0) {
        sprite_list->left = (char*)calloc(strlen(value) + 1, sizeof(char));
        strcpy(sprite_list->left, value);
    }
    else if (strcmp(attr, "right") == 0) {
        sprite_list->right = (char*)calloc(strlen(value) + 1, sizeof(char));
        strcpy(sprite_list->right, value);
    }
    else if (strcmp(attr, "up") == 0) {
        sprite_list->up = (char*)calloc(strlen(value) + 1, sizeof(char));
        strcpy(sprite_list->up, value);
    }
    else if (strcmp(attr, "down") == 0) {
        sprite_list->down = (char*)calloc(strlen(value) + 1, sizeof(char));
        strcpy(sprite_list->down, value);
    }
}
void load_player(char* k,int v, char* string_val, LevelConfig_t* c) {
    const char* prefix = "player_";
    if(strncmp(k, prefix, strlen(prefix)) != 0) return;
    char attr[MAX_ATTRIBUTE_LENGTH];
    sscanf(k + strlen(prefix), "%[^=]", attr);
    if (c->player == NULL) {
        c->player = (Player_t*)calloc(1,sizeof(Player_t));
        if(c->player == NULL) {
            exit(1);
        }
    }
    if (!strcmp(attr, "width")) c->player->obj.width = v;
    else if (!strcmp(attr, "height")) c->player->obj.height = v;
    else if (!strcmp(attr, "speed_x")) c->player->obj.speed_x = v;
    else if (!strcmp(attr, "speed_y")) c->player->obj.speed_y = v;
    else if (!strcmp(attr, "life_force")) c->player->life_force = v;
    else if (!strcmp(attr, "color")) c->player->obj.color = v;
    else if (!strcmp(attr, "taxi_cooldown")) c->player->taxi_cooldown = v;
    else if (!strcmp(attr, "base_taxi_cooldown")) c->player->base_taxi_cooldown = v;
    else if (!strcmp(attr, "blink_tick_ratio")) c->player->blink_tick_ratio = v;
    else {
        load_sprites(attr, string_val,&c->player->obj.sprites_list);
        load_blink_sprites(attr, string_val,&c->player->blink_sprites);
    }
}
void load_blink_sprites(char* attr, char* value,SpriteList_t* sprite_list) {
    if (strcmp(attr, "left_blink") == 0) {
        sprite_list->left = (char*)calloc(strlen(value) + 1, sizeof(char));
        strcpy(sprite_list->left, value);
    }
    else if (strcmp(attr, "right_blink") == 0) {
        sprite_list->right = (char*)calloc(strlen(value) + 1, sizeof(char));
        strcpy(sprite_list->right, value);
    }
    else if (strcmp(attr, "up_blink") == 0) {
        sprite_list->up = (char*)calloc(strlen(value) + 1, sizeof(char));
        strcpy(sprite_list->up, value);
    }
    else if (strcmp(attr, "down_blink") == 0) {
        sprite_list->down = (char*)calloc(strlen(value) + 1, sizeof(char));
        strcpy(sprite_list->down, value);
    }
}
void check_if_sprite_is_correct(GameObject_t* obj) {
    if (obj->sprites_list.left == NULL ||
        obj->sprites_list.right == NULL ||
        obj->sprites_list.up == NULL ||
        obj->sprites_list.down == NULL) {
        exit(1);
    }
    if(strlen(obj->sprites_list.left) != (unsigned int)((obj->width * obj->height)) ||
        strlen(obj->sprites_list.right) != (unsigned int)((obj->width * obj->height)) ||
        strlen(obj->sprites_list.up) != (unsigned int)((obj->width * obj->height)) ||
        strlen(obj->sprites_list.down) != (unsigned int)((obj->width * obj->height))) {
        printf("Sprite size is incorect than width*height\n");
        exit(1);
    }
}
void check_if_blink_loaded_correct(Player_t* p){
    if (p->blink_sprites.left == NULL ||
        p->blink_sprites.right == NULL ||
        p->blink_sprites.up == NULL ||
        p->blink_sprites.down == NULL) {
        exit(1);
    }
    if(strlen(p->blink_sprites.left) != (unsigned int)((p->obj.width * p->obj.height)) ||
        strlen(p->blink_sprites.right) != (unsigned int)((p->obj.width * p->obj.height)) ||
        strlen(p->blink_sprites.up) != (unsigned int)((p->obj.width * p->obj.height)) ||
        strlen(p->blink_sprites.down) != (unsigned int)((p->obj.width * p->obj.height))) {
        printf("Blink Sprite size is incorect than width*height\n");
        exit(1);
    }
}
int compare_scores(const void* a, const void* b) {
    LeaderboardEntry_t* entryA = (LeaderboardEntry_t*)a;
    LeaderboardEntry_t* entryB = (LeaderboardEntry_t*)b;
    return entryB->score - entryA->score;
}
LeaderboardEntry_t* load_leaderboard_entries(int entry_count) {
        char temp_name[PLAYER_NAME_MAX+1];
        FILE* file = fopen("ranking.txt", "r");
        LeaderboardEntry_t* entries = (LeaderboardEntry_t*)calloc((unsigned int)entry_count, sizeof(LeaderboardEntry_t));
        for(int i=0;i<entry_count;i++){
            entries[i].name = (char*)calloc(PLAYER_NAME_MAX+1,sizeof(char));
            if(entries[i].name == NULL){
                exit(1);
            }
        }
        int i = 0;
        while (i < entry_count && fscanf(file, "%d %s", &entries[i].score, temp_name) == 2) {
            strcpy(entries[i].name, temp_name); // Standard string copy
            i++;
        }
        fclose(file);
        qsort(entries, (size_t)entry_count, sizeof(entries[0]), compare_scores);
        return entries;
}
int save_to_leaderboard(char* player_name, int score){
    FILE* file = fopen("ranking.txt", "a");
    if (file == NULL) {
        return MALLOC_ERROR;
    }
    fprintf(file, "%d %s\n", score, player_name);
    fclose(file);
    return 0;
}
void free_level_config(LevelConfig_t* config) {
    if (config == NULL) return;
    for (int i = 0; i < config->hunter_type_count; i++) {
        free((void*)config->hunters[i].obj.sprites_list.left);
        free((void*)config->hunters[i].obj.sprites_list.right);
        free((void*)config->hunters[i].obj.sprites_list.up);
        free((void*)config->hunters[i].obj.sprites_list.down);
    }
    free((void*)config->hunters);
    free((void*)config->player->obj.sprites_list.left);
    free((void*)config->player->obj.sprites_list.right);
    free((void*)config->player->obj.sprites_list.up);
    free((void*)config->player->obj.sprites_list.down);
    free((void*)config->player->blink_sprites.left);
    free((void*)config->player->blink_sprites.right);
    free((void*)config->player->blink_sprites.up);
    free((void*)config->player->blink_sprites.down);
    free((void*)config->player);
    free((void*)config);
}