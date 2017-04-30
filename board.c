#include "boggle.h"
#include <stdlib.h>
#include <time.h>

// board, with a sensible default set of letters for testing.

char board[HEIGHT][WIDTH] = {{'S', 'U', 'S', 'G'},
                             {'J', 'E', 'R', 'Y'},
                             {'R', 'I', 'O', 'I'},
                             {'E', 'A', 'N', 'T'}};

/** Shuffle order of dice */

static void shuffle(const char *array[], size_t n) {
  for (size_t i = 0; i < n - 1; i++) {
    size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
    const char *t = array[j];
    array[j] = array[i];
    array[i] = t;
  }
}

/** Make random board. */

void make_board() {
  const char *dice[] = {
      "AAEEGN", "ELRTTY", "AOOTTW", "ABBJOO", "EHRTVW", "CIMOTU", "DISTTY",
      "EIOSST", "DELRVY", "ACHOPS", "HIMNQU", "EEINSU", "EEGHNW", "AFFKPS",
      "HLNNRZ", "DEILRX"};
  srandom((unsigned int) time(NULL));
  shuffle(dice, 16);

  for (int y = 0; y < HEIGHT; y++)
    for (int x = 0; x < WIDTH; x++)
      board[y][x] = dice[y * 4 + x][random() % 6];
}


