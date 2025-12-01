#include "game.h"

// co linijke fgets() sscanf by podzielić na key i value i zwrócić wartość dla target_key przypisując go do structa LevelConfig i osobno dla hunterów
void load_hunters(char key[64], int value,char string_val[256], LevelConfig_t* config, int count);
void load_sprites(char* attribute, char value[256], LevelConfig_t* config, int i);
char* maloc_sprite(LevelConfig_t* config, int i);

LevelConfig_t* load_level_config(int level_num) {
    LevelConfig_t* config = malloc(sizeof(LevelConfig_t));
    char filename[128];
    sprintf(filename, "data/level%d.conf", level_num);
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        exit(1);
    }
    char line[256], key[64];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        int value;
        char attribute[64];
        if (sscanf(line, "%[^=]=%d", key, &value)== 2) {
            if (strcmp(key, "map_rows") == 0) config->map_rows = value;
            else if (strcmp(key, "map_cols") == 0) config->map_cols = value;
            else if (strcmp(key, "status_rows") == 0) config->status_rows = value;
            else if (strcmp(key, "status_cols") == 0) config->status_cols = value;
            else if (strcmp(key, "time_limit_ms") == 0) config->time_limit_ms = value;
            else if (strcmp(key, "star_quota") == 0) config->star_quota = value;
            else if (strcmp(key, "player_health") == 0) config->player_health = value;
            else if (strcmp(key, "hunter_spawn_rate") == 0) config->hunter_spawn_rate = value;
            else if (strcmp(key, "hunter_type_count") == 0) {
                config->hunter_type_count = value;
                config->hunters = malloc(sizeof(Enemy_t) * value);
            }
            else if (strcmp(key, "seed") == 0) config->seed = value;
            if(config->hunter_type_count ) {
                load_hunters(key, value, attribute, config, config->hunter_type_count);
            }
        }
        else if(sscanf(line, "%[^=]=%s", key, attribute) == 2) {
            if(config->hunter_type_count ) {
                load_hunters(key,0,attribute, config, config->hunter_type_count);
            }
        }
    }
    fclose(file);
    return config;  
}
void load_hunters(char key[64], int value,char string_val[256], LevelConfig_t* config, int count) {
    for(int i=0; i < count; i++) {
        char prefix[32];
        sprintf(prefix, "hunter_%d_", i+1);
        if (strncmp(key, prefix, strlen(prefix)) != 0) continue;
        char attribute[32];
        sscanf(key + strlen(prefix), "%[^=]", attribute);
        if (strcmp(attribute, "width") == 0) config->hunters[i].obj.width = value;
        else if (strcmp(attribute, "height") == 0) config->hunters[i].obj.height = value;
        else if (strcmp(attribute, "speed_x") == 0) config->hunters[i].obj.speed_x = value;
        else if (strcmp(attribute, "speed_y") == 0) config->hunters[i].obj.speed_y = value;
        else if (strcmp(attribute, "bounces") == 0) config->hunters[i].bounces = value;
        else if (strcmp(attribute, "damage") == 0) config->hunters[i].damage = value;
        else {
            load_sprites(attribute, string_val, config, i);
        }
    }
}
void load_sprites(char* attribute, char value[256], LevelConfig_t* config, int i) {
    SpriteList_t* sprite_list= &config->hunters[i].obj.sprites_list;
    if (strcmp(attribute, "left") == 0) {
        sprite_list->left = malloc(sizeof(char) * (strlen(value) + 1));
        strcpy(sprite_list->left, value);
    }
    else if (strcmp(attribute, "right") == 0) {
        sprite_list->right = malloc(sizeof(char) * (strlen(value) + 1));
        strcpy(sprite_list->right, value);
    }
    else if (strcmp(attribute, "up") == 0) {
        sprite_list->up = malloc(sizeof(char) * (strlen(value) + 1));
        strcpy(sprite_list->up, value);
    }
    else if (strcmp(attribute, "down") == 0) {
        sprite_list->down = malloc(sizeof(char) * (strlen(value) + 1));
        strcpy(sprite_list->down, value);
    }
}
void free_level_config(LevelConfig_t* config) {
    if (config == NULL) return;
    for (int i = 0; i < config->hunter_type_count; i++) {
        free(config->hunters[i].obj.sprites_list.left);
        free(config->hunters[i].obj.sprites_list.right);
        free(config->hunters[i].obj.sprites_list.up);
        free(config->hunters[i].obj.sprites_list.down);
    }
    free(config->hunters);
    free(config);
}