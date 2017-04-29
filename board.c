#include "boggle.h"
#include <stdlib.h>
#include <time.h>
#include <memory.h>


char board[HEIGHT][WIDTH] = {{'M', 'E', 'A', 'T'},
                             {'O', 'Z', 'Z', 'Z'},
                             {'M', 'E', 'N', 'Z'},
                             {'Z', 'Z', 'Z', 'T'}};

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

void sort(char array[4], size_t n) {
    bool is_sorted;
    do {
        is_sorted = true;
        n -= 1;
        for (unsigned int i = 0; i < n; ++i)
            if (strcmp(&array[i], &array[i + 1]) == 1) {
                is_sorted = false;
                char temp = array[i];
                array[i] = array[i + 1];
                array[i + 1] = temp;
            }
    } while (!is_sorted);
}

/** Make random board. */

void make_board() {
    srandom((unsigned int) time(NULL));
    shuffle(dice, 16);

    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            board[i][j] = dice[i * 4 + j][random() % 6];

    sort(board[0], 16);
}

void display_board() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            mvwaddch(wboard, i * 2 + 1, j * 4 + 2, (const chtype) board[i][j]);
        }
    }
}

#if 0
int main() {
  make_board();
  display_board();
}
#endif
