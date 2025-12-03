int calculate_damage(int damage, int time_max, int time_left, int dmg_mul){
    const int time_factor = (time_max - time_left) / (time_max / dmg_mul);
    return damage+(time_factor * dmg_mul)*damage;
}