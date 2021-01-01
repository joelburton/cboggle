/** cboggle: a straightforward boggle game using ncurses and a DAWG. */

#include <stdbool.h>
#include <ncurses.h>
#include <err.h>
#include <search.h>

#ifndef BOGGLE_H
#define BOGGLE_H

// Given that we use a 64-bit mask to keep track of tiles used, the board
// cannot have more than 64 spots. Non-square boards are unconventional but
// should work.
#define HEIGHT 4
#define WIDTH 4
#define WORDS_PATH "/usr/local/share/boggle/words.dat"
#define WORDS_LOCAL_PATH "words.dat"
#define FATAL2(m, m2) \
  err(1, "%s:%i: %s: %s %s", __FILE__, __LINE__, __func__, m, m2)

typedef struct BoardWord_s {
    const char *word;
    bool found;
} BoardWord;

extern BoardWord *legal;
extern int board_nwords;
extern int board_longest;
extern int board_score;
extern int player_nwords;
extern int player_score;

// board.c

extern char board[HEIGHT][WIDTH];

void make_board();

// check.c

void find_all_words();

void free_words();

int guess_word(char word[]);

// dict.c

void read_all(bool local);

extern const int32_t *dawg;

// ui.c

extern int round_length;

// util.c

#endif /* end of boggle_h */
