#include "game.h"

/*int main(){
    init_ncurses();
    Win* main_win;
    Win* status_win;
    create_window(main_win, 20, 40, 0, 0);
    create_window(status_win, 20, 40, 20, 0);
    
    refresh();

    draw_border(main_win);
    draw_border(status_win);
    wnoutrefresh(main_win->window);
    wnoutrefresh(status_win->window);
    doupdate();
    while(getch()!='q'){
        
    }

    delwin(main_win->window);
    delwin(status_win->window);
    endwin();
    return 0;
}*/
int game_loop(Win* main_win, Win* status_win, Player_t* player, Map_t map){
    char input;
    while (1)
    {
        nodelay(stdscr, TRUE);
        input = getch();
        while (getch() != ERR);
        nodelay(stdscr, FALSE);
        if(input == 'q'){
            return 0;
        }
        map[0][0]='X';
        handle_player_input(player, input);
        draw_player(player, main_win);
        werase(main_win->window);
        wnoutrefresh(main_win->window);
        mount_upd(status_win);
        doupdate();
        usleep(100);
    }
    
    return 0;
}
int main(){
    init_ncurses();
    Win* main_win=create_window(20, 40, 0, 0);
    Win* status_win=create_window(10, 40, 20, 0);
    refresh();
    mount_upd(main_win);
    mount_upd(status_win);
    Map_t map=create_map(main_win->rows,main_win->cols);
    Player_t* player=create_player();
    draw_player(player,main_win);
    wnoutrefresh(main_win->window);
    doupdate();
    if(!game_loop(main_win,status_win,player,map)){
        //endgame condition
    }
    destroy_player(player);
    delwin(main_win->window);
    delwin(status_win->window);
    endwin();
    free_map(map,main_win->rows);
    free(main_win);
    free(status_win);
}