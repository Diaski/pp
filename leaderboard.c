#include "game.h"
#include <stdlib.h>
#include <string.h>

int compare_scores(const void* a, const void* b) {
    const LeaderboardEntry_t* entryA = (LeaderboardEntry_t*)a;
    const LeaderboardEntry_t* entryB = (LeaderboardEntry_t*)b;
    return entryB->score - entryA->score;
}

LeaderboardEntry_t* load_leaderboard_entries(int entry_count) {
        char temp_name[PLAYER_NAME_MAX+1];
        FILE* file = fopen("ranking.txt", "r");
        LeaderboardEntry_t* entries = (LeaderboardEntry_t*)calloc((unsigned int)entry_count, sizeof(LeaderboardEntry_t));
        for(int i=0;i<entry_count;i++){
            entries[i].name = (char*)calloc(PLAYER_NAME_MAX+1,sizeof(char));
            if(entries[i].name == NULL){
                exit(MALLOC_ERROR);
            }
        }
        int i = 0;
        while (i < entry_count && fscanf(file, "%d %[^\n]", &entries[i].score, temp_name) == 2) {
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
    if(score <=0){
        fclose(file);
        return 0;
    }
    fprintf(file, "%d %s\n", score, player_name);
    fclose(file);
    return 0;
}

int count_leaderboard_entries(void){
    FILE* file = fopen("ranking.txt", "r");
    if (file == NULL) {
        return 0;
    }
    int count = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        count++;
    }
    fclose(file);
    return count;
}

void free_leaderboard_entries(LeaderboardEntry_t* entries, int entry_count){
    for(int i=0;i<entry_count;i++){
        free((void*)entries[i].name);
    }
    free((void*)entries);
    return;
}
