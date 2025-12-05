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