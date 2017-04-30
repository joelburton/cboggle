#include "boggle.h"
#include <stdlib.h>
#include <ctype.h>

int winrow, wincol;
int wwords_row, wwords_col;
WINDOW *wboard;
WINDOW *wwords;
WINDOW *wprompt;
WINDOW *wtimer;

/** Prompt for a keypress -- used for status messages */

static int prompt(const char *msg) {
  werase(wprompt);
  box(wprompt, 0, 0);
  mvwprintw(wprompt, 1, 2, msg);
  return wgetch(wprompt);
}

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

#define COL_WIDTH 15

static void print_words(bool show_found, bool show_not_found) {
  const int rows = wwords_row - 2;
  const int cols = wwords_col / COL_WIDTH;
  const int nresults = g_sequence_get_length(legal);
  int i = 0;

  GSequenceIter *p = g_sequence_get_begin_iter(legal);

  while (i < nresults) {
    int c = 0;
    werase(wwords);
    if (i > 0)
      prompt("Press any key to see more words ");

    while (c < (rows * cols) && i < nresults) {
      BoardWord *bw = g_sequence_get(p);

      if ((show_found && bw->found) || (show_not_found && !bw->found)) {
        int y = c % rows + 1;
        int x = (c / rows) * COL_WIDTH + 2;
        mvwprintw(wwords, y, x, "%s\n", bw->word);
        c += 1;
      }
      i++;
      p = g_sequence_iter_next(p);
    }
    box(wwords, 0, 0);
    wrefresh(wwords);
  }
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

/** Get a word, while updating timer. */

static bool get_word(char *word, time_t start_round) {
  int wl = 0;
  char *blank = "                 ";
  mvwaddstr(wprompt, 1, 4, blank);
  halfdelay(1);

  while (true) {
    int ch = mvwgetch(wprompt, 1, 4 + wl);

    int left = (int) (round_length - (time(NULL) - start_round));
    mvwprintw(wtimer, 0, 0, "Secs: %3d", left);
    wrefresh(wtimer);

    if (left <= 0 || ch == '*')
      return true;

    if (ch == ERR)
      // ran out of time so timer updated; keep going
      continue;

    if (ch == '\n') {
      // finished word, mark as done and return
      word[wl] = '\0';
      return false;
    }

    if (ch == KEY_BACKSPACE || ch == KEY_LEFT || ch == 127) {
      if (wl > 0)
        mvwaddch(wprompt, 1, 4 + (--wl), ' ');
    }

    if (wl >= 16)
      // word is max length
      continue;

    if (ch >= 'A' && ch <= 'Z')
      ch = tolower(ch);

    if (ch >= 'a' && ch <= 'z')
      word[wl++] = (char) ch;

    mvwaddnstr(wprompt, 1, 4, word, wl);
  }
}

/** Play a round of user input. */

static void player_round() {
  werase(wprompt);
  box(wprompt, 0, 0);
  mvwprintw(wprompt, 1, 2, "> ");
  char word[17];
  time_t start_round = time(NULL);
  halfdelay(1);

  while (true) {
    if (get_word(word, start_round))
      // ran out of time, so round is over
      break;

    (void) guess_word(word);
    print_words(true, false);
  }

  cbreak();
}

/** Play a single board. */

static void play_board() {
  make_board();
  read_all();
  find_all_words();

  werase(wboard);
  box(wboard, 0, 0);
  wrefresh(wboard);

  mvprintw(3, 19, "Type * to stop");
  mvprintw(10, 1, "Correctly Guessed Words:");
  refresh();
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

int main(int argc, char *argv[]) {

  if (argc > 1)
    round_length = atoi(argv[1]);
  else
    round_length = 300;

  initscr();
  signal(SIGINT, finish);
  cbreak();
  noecho();

  getmaxyx(stdscr, winrow, wincol);

  mvprintw(1, 19, "LOVELY LEVERET LEXIGAME v1.0");
  refresh();

  wtimer = newwin(1, 10, 3, 38);
  wboard = newwin(9, 17, 0, 0);
  wprompt = newwin(3, wincol - 20, 6, 19);
  keypad(wprompt, TRUE);

  wwords_row = winrow - 11;
  wwords_col = wincol;

  wwords = newwin(wwords_row, wwords_col, 11, 0);

  do
    play_board();
  while (toupper(prompt("Play again? ")) == 'Y');

  finish(0);
}
