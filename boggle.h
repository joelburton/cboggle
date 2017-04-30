#include <stdbool.h>
#include <ncurses.h>
#include <glib.h>

#ifndef BOGGLE_H
#define BOGGLE_H

// Given that we use a 64-bit mask to keep track of tiles used, the board
// cannot have more than 64 spots. Non-square boards are unconventional but
// should work.
#define HEIGHT 4
#define WIDTH 4
#define WORDS_PATH "3words.txt"


typedef struct Trie {
  bool term;
  struct Trie *letters[26];
} Trie;

Trie *trie;


typedef struct BoardWord_s {
  const char *word;
  bool found;
} BoardWord;

GSequence *legal;


// board.c

char board[HEIGHT][WIDTH];
void make_board();

// check.c

void find_all_words();
void free_words();
int guess_word(char word[]);

// dict.c

void read_all();

// ui.c

int round_length;


#endif /* end of boggle_h */
