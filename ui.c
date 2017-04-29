#include "boggle.h"
#include <signal.h>
#include <stdlib.h>

void finish(int sig);

WINDOW *window;

_Noreturn int main() {
    window = initscr();
    signal(SIGINT, finish);
    cbreak();
    // noecho();
    getmaxyx(stdscr, winrow, wincol);
    make_board();
    check_all();

    // mvprintw(1,19, "LOVELY LEVERET LEXIGAME v1.0");
    mvprintw(1, 19, "BOGGLE v1.0");
    mvprintw(3, 19, "Enter '*' to quit round.");

    mvprintw(10, 1, "Correctly Guessed Words:");
    refresh();

    wboard = newwin(9, 17, 0, 0);
    box(wboard, 0, 0);
    display_board();
    wrefresh(wboard);

    wprompt = newwin(3, wincol - 19, 6, 19);


    wwords_row = winrow - 11;
    wwords_col = wincol;

    wwords = newwin(wwords_row, wwords_col, 11, 0);
    print_words(wwords, true);

    char word[20];
    while (true) {
        werase(wprompt);
        box(wprompt, 0, 0);
        mvwprintw(wprompt, 1, 2, "> ");
        wgetstr(wprompt, word);
        if (word[0] == '*') break;
        (void) guess_word(word);

        print_words(wwords, true);
    }

    werase(wprompt);
    box(wprompt, 0, 0);
    mvwprintw(wprompt, 1, 2, "Press any key to see missed words ");
    wgetch(wprompt);

    mvprintw(10, 1, "Missed Words:");
    clrtoeol();
    refresh();

    clrtoeol();
    print_words(wwords, false);

    werase(wprompt);
    box(wprompt, 0, 0);
    mvwprintw(wprompt, 1, 2, "Press any key to quit ");
    wgetch(wprompt);

    finish(0);
}

_Noreturn void finish(int sig __attribute__ ((unused))) {
    endwin();
    free_words();
    delwin(wwords);
    delwin(wboard);
    delwin(wprompt);
    delwin(window);
    exit(EXIT_SUCCESS);
}
