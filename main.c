#include "game.h"

int main(){
    init_ncurses();
    char* player_name = player_name_window();
    int res =0;
    while(res ==0){
        res = level_selector(player_name);
        if (res == WINNER){
            res =0;
            // tu będzie jakaś logika wygrania typu ekran zwycięstwa itd
        }
        if (res == PLAYER_DIED){
            res = 0;
            // tu będzie jakaś logika przegrania typu ekran śmierci itd
        }
    }
    free(player_name);
    if (res == 2){
        printf("Error loading level configuration.\n");
    } else if (res ==3){
        printf("Terminal size is too small for the selected level.\n");
    }
}