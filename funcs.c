#include "game.h"
void copy_sprite(Sprite_t dest, const Sprite_t src){
    strcpy(dest,src);
}
void calculate_damage(Enemy_t* enemy, int time_max, int time_left, int dmg_mul){
    int time_factor = (time_max - time_left) / (time_max / dmg_mul);
    enemy->damage += (time_factor * dmg_mul)*enemy->damage;
}