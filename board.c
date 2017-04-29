#include "boggle.h"
#include <stdlib.h>
#include <time.h>


char board[HEIGHT][WIDTH] = {{'S', 'U', 'S', 'G'},
                             {'J', 'E', 'R', 'Y'},
                             {'R', 'I', 'O', 'I'},
                             {'E', 'A', 'N', 'T'}};

char *dice[] = {"AAEEGN", "ELRTTY", "AOOTTW", "ABBJOO", "EHRTVW", "CIMOTU",
                "DISTTY", "EIOSST", "DELRVY", "ACHOPS", "HIMNQU", "EEINSU",
                "EEGHNW", "AFFKPS", "HLNNRZ", "DEILRX"};

void shuffle(char *array[], size_t n) {
    for (size_t i = 0; i < n - 1; i++) {
        size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
        char *t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

/** Make random board. */

void make_board() {
    srandom((unsigned int) time(NULL));
    shuffle(dice, 16);

    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            board[i][j] = dice[i * 4 + j][random() % 6];
}

void display_board() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            mvwaddch(wboard, i * 2 + 1, j * 4 + 2, (const chtype) board[i][j]);
        }
    }
}

