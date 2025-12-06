#include "game.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


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
    for(int i = 0; i < config->hunter_type_count; i++) {
        check_if_sprite_is_correct(&config->hunters[i].obj);
    }
    return config;  
}
void base_asignements(LevelConfig_t* config, char* key,int value) {
    if (strcmp(key, "map_rows") == 0) config->map_rows = value;
    if (strcmp(key, "map_cols") == 0) config->map_cols = value;
    if (strcmp(key, "status_rows") == 0) config->status_rows = value;
    if (strcmp(key, "status_cols") == 0) config->status_cols = value;
    if (strcmp(key, "time_limit_ms") == 0) config->time_limit_ms = value;
    if (strcmp(key, "star_quota") == 0) config->star_quota = value;
    if (strcmp(key, "hunter_spawn_rate") == 0) config->hunter_spawn_rate = value;
    if (strcmp(key, "max_speed") == 0) config->max_speed = value;
    if (strcmp(key, "delta_speed") == 0) config->delta_speed = value;
    if (strcmp(key, "min_speed") == 0) config->min_speed = value;
    if (strcmp(key, "damage_over_time_mult") == 0) config->damage_over_time_mult = value;
    if (strcmp(key, "score_time_bias") == 0) config->score_time_bias = value;
    if (strcmp(key, "score_star_bias") == 0) config->score_star_bias = value;
    if (strcmp(key, "seed") == 0) config->seed = value;
    if (strcmp(key, "max_enemys_per_level") == 0) config->max_enemys_per_level = value;
    if (strcmp(key, "star_spawn_chance") == 0) config->star_spawn_chance = value;
    if (strcmp(key, "base_score") == 0) config->base_score = value;
}

void assign_values(LevelConfig_t* config, char* key,char* line) {
        int value;
        char attribute[MAX_ATTRIBUTE_LENGTH];
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
        else if(sscanf(line, "%[^=]=%s", key, attribute) == 2) {
            if(config->hunter_type_count ) {
                load_hunters(key,0,attribute, config, config->hunter_type_count);
                load_player(key, 0, attribute, config);
            }
        }
}

void load_hunters(char* key, int value,char* string_val, LevelConfig_t* config, int count) {
    for(int i=0; i < count; i++) {
        char prefix[MAX_PREFIX_LENGTH];
        snprintf(prefix, sizeof(prefix), "hunter_%d_", i+1);
        if (strncmp(key, prefix, strlen(prefix)) != 0) continue;
        char attribute[MAX_ATTRIBUTE_LENGTH];
        sscanf(key + strlen(prefix), "%[^=]", attribute);
        if (strcmp(attribute, "width") == 0) config->hunters[i].obj.width = value;
        else if (strcmp(attribute, "height") == 0) config->hunters[i].obj.height = value;
        else if (strcmp(attribute, "speed_x") == 0) config->hunters[i].obj.speed_x = value;
        else if (strcmp(attribute, "speed_y") == 0) config->hunters[i].obj.speed_y = value;
        else if (strcmp(attribute, "bounces") == 0) config->hunters[i].bounces = value;
        else if (strcmp(attribute, "damage") == 0) config->hunters[i].damage = value;
        else if (strcmp(attribute, "color") == 0) config->hunters[i].obj.color = value;
        else if (strcmp(attribute, "dash_limit") == 0) config->hunters[i].dash_limit = value;
        else if (strcmp(attribute, "base_dash_sleep_time") == 0) config->hunters[i].base_dash_sleep_time = value;
        else {
            load_sprites(attribute, string_val,&config->hunters[i].obj.sprites_list);
        }
    }
}
void load_sprites(char* attribute, char* value,SpriteList_t* sprite_list) {
    if (strcmp(attribute, "left") == 0) {
        sprite_list->left = (char*)calloc(strlen(value) + 1, sizeof(char));
        strcpy(sprite_list->left, value);
    }
    else if (strcmp(attribute, "right") == 0) {
        sprite_list->right = (char*)calloc(strlen(value) + 1, sizeof(char));
        strcpy(sprite_list->right, value);
    }
    else if (strcmp(attribute, "up") == 0) {
        sprite_list->up = (char*)calloc(strlen(value) + 1, sizeof(char));
        strcpy(sprite_list->up, value);
    }
    else if (strcmp(attribute, "down") == 0) {
        sprite_list->down = (char*)calloc(strlen(value) + 1, sizeof(char));
        strcpy(sprite_list->down, value);
    }
}
void load_player(char* key,int value, char* string_val, LevelConfig_t* config) {
    const char* prefix = "player_";
    if(strncmp(key, prefix, strlen(prefix)) != 0) return;
    char attribute[MAX_ATTRIBUTE_LENGTH];
    sscanf(key + strlen(prefix), "%[^=]", attribute);
    if (config->player == NULL) {
        config->player = (Player_t*)calloc(1,sizeof(Player_t));
        if(config->player == NULL) {
            exit(1);
        }
    }
    if (strcmp(attribute, "width") == 0) config->player->obj.width = value;
    else if (strcmp(attribute, "height") == 0) config->player->obj.height = value;
    else if (strcmp(attribute, "speed_x") == 0) config->player->obj.speed_x = value;
    else if (strcmp(attribute, "speed_y") == 0) config->player->obj.speed_y = value;
    else if (strcmp(attribute, "life_force") == 0) config->player->life_force = value;
    else if (strcmp(attribute, "color") == 0) config->player->obj.color = value;
    else if (strcmp(attribute, "taxi_cooldown") == 0) config->player->taxi_cooldown = value;
    else if (strcmp(attribute, "base_taxi_cooldown") == 0) config->player->base_taxi_cooldown = value;
    else load_sprites(attribute, string_val,&config->player->obj.sprites_list);
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
    free((void*)config->player);
    free((void*)config);
}