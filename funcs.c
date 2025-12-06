#include "game.h"

int calculate_damage(int damage, int time_max, int time_left, int dmg_mul) {
    long long top = (long long)damage * time_max * dmg_mul;
    long long bottom = time_max + time_left;
    return (int)(top / bottom);
}

int calculate_bounces(int bounces, int time_max, int time_left) {
    long long top = (long long)bounces * time_max * BOUNCES_MULT_OVER_TIME;
    long long bottom = time_max + time_left;
    
    return (int)(top / bottom);
}
int calculate_score(LevelConfig_t* level_config,Player_t* player,int time_max, int time_left) {
    int score_from_time = (int)(((long long)level_config->score_time_bias * time_max) / (time_max + time_left));
    int score_from_stars = level_config->score_star_bias * player->stars;
    if(player->life_force <=0){
        return 0;
    }
    return score_from_time * score_from_stars * player->life_force/LIFE_FORCE_TO_SCORE_DIVIDER * level_config->base_score;
}