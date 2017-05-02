/** cboggle: a straightforward boggle game using ncurses and a DAWG. */

#include <stdbool.h>
#include <ncurses.h>
#include <glib.h>
#include <err.h>

#ifndef BOGGLE_H
#define BOGGLE_H

// Given that we use a 64-bit mask to keep track of tiles used, the board
// cannot have more than 64 spots. Non-square boards are unconventional but
// should work.
#define HEIGHT 4
#define WIDTH 4
#define WORDS_PATH "words.dat"
#define FATAL(m) err(1, "%s -- %s:%i %s()", m, __FILE__, __LINE__, __func__)

typedef struct BoardWord_s {
  const char *word;
  bool found;
} BoardWord;

GSequence *legal;
int board_nwords;
int board_score;
int player_nwords;
int player_score;

// board.c

char board[HEIGHT][WIDTH];
void make_board();

// check.c

void find_all_words();
void free_words();
int guess_word(char word[]);

// dict.c

void read_all();
const int32_t * dawg;

// ui.c

int round_length;
_Noreturn void fatal(const char *func);

// util.c
char *strip_whitespace(char *str);
_Noreturn void fatal(const char *func);


#endif /* end of boggle_h */
