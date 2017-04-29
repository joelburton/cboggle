#include <stdbool.h>
#include <ncurses.h>

#ifndef BOGGLE_H
#define BOGGLE_H

// Given that we use a 64-bit mask to keep track of tiles used, the board
// cannot have more than 64 spots. Non-square boards are unconventional but
// should work.
#define HEIGHT 4
#define WIDTH 4
#define NTILES HEIGHT * WIDTH
#define NTILES_STR "16"
#define WORDS_PATH "/usr/share/dict/words"

char board[HEIGHT][WIDTH];

typedef struct BoardWord_s {
  char *word;
  bool found;
  struct BoardWord_s *next;
} BoardWord;

BoardWord *legal;

int winrow, wincol;
int wwords_row, wwords_col;
WINDOW *wboard;
WINDOW *wwords;
WINDOW *wprompt;

void make_board();
void display_board();
bool find_word(const char word[]);
void check_all();
int guess_word(char word[]);
void print_words(WINDOW *win, bool guessed);
void free_words();
void read_all();


#endif /* end of boggle_h */
