#include "boggle.h"
#include <signal.h>
#include <stdlib.h>
#include <ctype.h>

int winrow, wincol;
int wwords_row, wwords_col;
WINDOW *wboard;
WINDOW *wwords;
WINDOW *wprompt;

/** Display board in curses window. */

static void display_board() {
  for (int y = 0; y < HEIGHT; y++)
    for (int x = 0; x < WIDTH; x++)
      // horiz: space letters out by 4, vert: by 2
      mvwaddch(wboard, y * 2 + 1, x * 4 + 2, (const chtype) board[y][x]);
}

/** Print words to curses window.
 *
 * @param found  Show found words? (If not, not-found words)
 */

static void print_words(bool show_found, bool show_not_found) {
  BoardWord *p = legal;
  int i = 0;
  int rows = wwords_row - 2;
  werase(wwords);

  while (p != NULL) {
    if ((show_found && p->found) || (show_not_found && !p->found)) {
      int y = i % rows + 1;
      int x = (i / rows) * 15 + 2;
      mvwprintw(wwords, y, x, "%s\n", p->word);
      i++;
    }

    p = p->next;
  }

  box(wwords, 0, 0);
  wrefresh(wwords);
}

/** Finish program.
 *
 * @param sig signal
 *
 * This is a separate function so it can be used both at a clean exit, but
 * also if Control-C is hit. We need to clean up curses stuff in order to
 * quit cleanly.
 */

_Noreturn void finish(int sig __attribute__ ((unused))) {
  endwin();
  exit(EXIT_SUCCESS);
}

/** Play a round of player guessing. */

static void player_round() {
  char word[20];
  while (true) {
    werase(wprompt);
    box(wprompt, 0, 0);
    mvwprintw(wprompt, 1, 2, "> ");
    wgetstr(wprompt, word);
    if (word[0] == '*')
      break;
    (void) guess_word(word);

    print_words(true, false);
  }
}

/** Prompt for a keypress -- used for status messages */

static int prompt(const char *msg) {
  werase(wprompt);
  box(wprompt, 0, 0);
  mvwprintw(wprompt, 1, 2, msg);
  return wgetch(wprompt);
}

/** Play a single board. */

static void play_board() {
  make_board();
  read_all();
  find_all_words();

  werase(wboard);
  box(wboard, 0, 0);
  wrefresh(wboard);

  mvprintw(3, 19, "Enter '*' to quit round.");
  mvprintw(10, 1, "Correctly Guessed Words:");
  print_words(false, false);
  prompt("Press any key to start. ");

  display_board();
  wrefresh(wboard);

  player_round();

  prompt("Press any key to see missed words ");
  mvprintw(10, 1, "Missed Words:");
  clrtoeol();
  refresh();

  print_words(false, true);
  free_words();
}

/** Main program. */

int main() {
  initscr();
  signal(SIGINT, finish);
  cbreak();
  getmaxyx(stdscr, winrow, wincol);

  mvprintw(1, 19, "LOVELY LEVERET LEXIGAME v1.0");
  refresh();

  wboard = newwin(9, 17, 0, 0);
  wprompt = newwin(3, wincol - 19, 6, 19);

  wwords_row = winrow - 11;
  wwords_col = wincol;

  wwords = newwin(wwords_row, wwords_col, 11, 0);

  do
    play_board();
  while (toupper(prompt("Play again? ")) == 'Y');

  finish(0);
}
