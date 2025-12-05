#include "game.h"
int calculate_damage(int damage, int time_max, int time_left, int dmg_mul){
    const int time_factor = (time_max - time_left) / (time_max / dmg_mul);
    return damage+(time_factor * dmg_mul)*damage;
}
int calculate_bounces(int bounces,int time_max,int time_left){
    return (bounces + (time_max * BOUNCES_MULT_OVER_TIME)/(time_max+time_left));
}