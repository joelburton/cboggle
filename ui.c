#include "boggle.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <time.h>
#include <signal.h>

BoardWord *legal;
int board_score;
int board_nwords;
int board_longest;
int player_nwords;
int player_score;
int round_length;

int winrow, wincol;
int wwords_row, wwords_col;
WINDOW *wboard;
WINDOW *wwords;
WINDOW *wprompt;
WINDOW *wtimer;


/** Prompt for a keypress -- used for status messages */

static int prompt_yn(const char *msg) {
    werase(wprompt);
    box(wprompt, 0, 0);
    mvwprintw(wprompt, 1, 2, msg);
    echo();
    while (true) {
        char buffer[4], answer[4];
        mvwgetnstr(wprompt, 1, 2 + strlen(msg), buffer, 3);
        sscanf(buffer, "%s", answer);

        if (strcasecmp(answer, "y") == 0 || strncasecmp(answer, "yes", 3) == 0) {
            noecho();
            return true;
        }
        if (strcasecmp(answer, "n") == 0 || strncasecmp(answer, "no", 2) == 0) {
            noecho();
            return false;
        }
        beep();
    }
}

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

/** State information about word-list printing.
 *
 * We need a global var for this, since the binary tree walking interface
 * uses a callback and provides no way to pass state in.
 */

struct PrintStatus {
    int rows;
    int cols;
    int c;      // # of items printed on this page
    bool show_found;
    bool show_not_found;
} ps;

/** Print a node -- called during traversal. */

#define COL_WIDTH 15

void printNode(const void *n, VISIT value, int level __attribute__((unused))) {
    if (value == leaf || value == postorder) {
        const BoardWord *bw = *(const BoardWord **) n;
        if ((ps.show_found && bw->found) || (ps.show_not_found && !bw->found)) {

            if (ps.c == ps.rows * ps.cols) {
                box(wwords, 0, 0);
                wrefresh(wwords);
                prompt("Press any key to see more words ");
                ps.c = 0;
                werase(wwords);
            }

            int y = ps.c % ps.rows + 1;
            int x = (ps.c / ps.rows) * COL_WIDTH + 2;
            mvwaddstr(wwords, y, x, bw->word);
            ps.c += 1;
        }
    }
}

/** Print words to curses window.
 *
 * @param show_found
 * @param show_not_found
 */

void print_words(bool show_found, bool show_not_found) {
    ps.rows = wwords_row - 2;
    ps.cols = wwords_col / COL_WIDTH;
    ps.c = 0;
    ps.show_found = show_found;
    ps.show_not_found = show_not_found;

    werase(wwords);
    twalk(legal, printNode);
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

_Noreturn void finish(int sig __attribute__((unused))) {
    endwin();
    exit(sig + 128);
}

/** Get a word, while updating timer. */

static bool get_word(char *word, char *prev, time_t start_round) {
    int wl = 0;
    halfdelay(1);

    while (true) {
        mvwprintw(wprompt, 1, 4, "%-16s", word);
        int ch = mvwgetch(wprompt, 1, 4 + wl);

        int left = (int) (round_length - (time(NULL) - start_round));
        mvwprintw(wtimer, 0, 0, "Secs: %3d", left);
        wrefresh(wtimer);

        if (left <= 0 || ch == 4) // ran out of time or CTRL-D
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
                word[--wl] = '\0';
        }

        // Next two are lightweight-history; can up/down arrow
        // to move from this word to prev
        if (ch == KEY_UP && prev[0] != '\0') {
            strcpy(word, prev);
            prev[0] = '\0';
            wl = (int) strlen(word);
        }

        if (ch == KEY_DOWN && prev[0] == '\0') {
            strcpy(prev, word);
            word[0] = '\0';
            wl = 0;
        }

        if (wl < 16) {
            if (ch >= 'A' && ch <= 'Z')
                ch = tolower(ch);

            if (ch >= 'a' && ch <= 'z')
                word[wl++] = (char) ch;
        }

        word[wl] = '\0';
    }
}

/** Play a round of user input. */

static void player_round() {
    player_nwords = 0;
    player_score = 0;

    werase(wprompt);
    box(wprompt, 0, 0);
    mvwprintw(wprompt, 1, 2, "> ");
    char word[17];
    char prev[17];
    prev[0] = '\0';

    time_t start_round = time(NULL);
    halfdelay(1);

    while (true) {
        word[0] = '\0';
        if (get_word(word, prev, start_round))
            // ran out of time, so round is over
            break;

        // guess_word returns a status, but we don't need to care
        (void) guess_word(word);
        mvprintw(10, 1, "Correctly Guessed Words: %d, Score: %d", player_nwords,
                 player_score);
        refresh();
        print_words(true, false);
        strcpy(prev, word);
    }

    cbreak();
}

/** Play a single board. */

static void play_board() {
    make_board();
    board_nwords = 0;
    board_score = 0;

    find_all_words();

    werase(wboard);
    box(wboard, 0, 0);
    wrefresh(wboard);

    refresh();
    print_words(false, false);
    prompt("Press any key to start. ");

    display_board();
    wrefresh(wboard);
    mvprintw(3, 19, "Words: %d (longest: %d), score: %d",
        board_nwords, board_longest, board_score);
    clrtoeol();
    refresh();

    player_round();

    if (prompt_yn("Do you want to see missed words? ")) {
        mvprintw(10, 1, "Missed Words:");
        clrtoeol();
        refresh();
        print_words(false, true);
    }

    free_words();
}

/** Main program. */

int main(int argc, char *argv[]) {

    if (argc > 1)
        round_length = atoi(argv[1]); // NOLINT(cert-err34-c)
    else
        round_length = 300;

    // determine if this is being run locally -- if so, we look
    // for the dictionary here

    bool local = strcmp(argv[0], basename(argv[0])) != 0;
    read_all(local);

    initscr();
    signal(SIGINT, finish);
    signal(SIGTERM, finish);
    cbreak();
    noecho();

    getmaxyx(stdscr, winrow, wincol);

    mvprintw(1, 19, "LOVELY LEVERET LEXIGAME v1.2");
    refresh();

    wtimer = newwin(1, 10, 3, 58);
    wboard = newwin(9, 17, 0, 0);
    wprompt = newwin(3, wincol - 19, 6, 19);
    keypad(wprompt, TRUE);

    wwords_row = winrow - 11;
    wwords_col = wincol;

    wwords = newwin(wwords_row, wwords_col, 11, 0);

    do
        play_board();
    while (prompt_yn("Play again? "));

    endwin();
    puts("\nHop along, little leveret!\n");
    exit(EXIT_SUCCESS);
}
