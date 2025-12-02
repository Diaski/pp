#include "game.h"
void copy_sprite(Sprite_t dest, const Sprite_t src){
    strcpy(dest,src);
}
void calculate_damage(Enemy_t* enemy, int time_max, int time_left, int dmg_mult){
    int time_factor = (time_max - time_left) / (time_max / dmg_mult);
    enemy->damage *= (time_factor * dmg_mult);
}