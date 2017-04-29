#include "boggle.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/** Can we find the rest of this word, starting at i,j?
 *
 * Look for the word, starting at tile [i,j]. Check to ensure
 * that this is a valid board square, that it matches the first
 * letter of the word sought, and that we have used this tile
 * before (track this by using `used` as a bitfield).
 * If this word is still possible, recurse and check in all directions
 * until this either succeeds or fails.
 */

bool find_rest(char word[], int i, int j, int_least64_t used) {

    // If not a legal tile, can't make word here
    if (i < 0 || i >= HEIGHT || j < 0 || j >= WIDTH)
        return false;

    // Doesn't match start of word, can't make word here
    if (board[i][j] != toupper(word[0]))
        return false;

    // Make bitmask for this tile position
    int_least64_t mask = 0x1 << (i * WIDTH + j);

    // If we've already used this tile, can't make word here
    if (used & mask)
        return false;

    // Was that the last letter? If so, we found it
    if (*(++word) == '\0')
        return true;

    // Mark tile as used
    used |= mask;

    // Check every direction H/V/D from here (will also re-check this tile, but
    // the can't-reuse-this-tile rule prevents it from actually succeeding)
    for (int di = -1; di < 2; di++)
        for (int dj = -1; dj < 2; dj++)
            if (find_rest(word, i + di, j + dj, used))
                return true;

    return false;
}

/** Can this word be found on the board? */

bool find_word(char word[]) {
    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            if (find_rest(word, i, j, 0x0))
                return true;

    return false;
}

bool add_word(char word[]) {
    BoardWord *new_word = malloc(sizeof(BoardWord));
    strcpy(new_word->word, word);
    new_word->found = false;
    new_word->next = NULL;

    if (legal == NULL) {
        // First legal word!
        legal = new_word;
        return true;
    }

    BoardWord *p = legal;
    while (p->next != NULL) {
        if (strcmp(p->word, word) == 0) {
            free(new_word); /// didn't need it after all
            return false;
        }
        p = p->next;
    }

    p->next = new_word;
    return true;
}

void free_words() {
    while (legal != NULL) {
        BoardWord *next = legal->next;
        free(legal);
        legal = next;
    }
}

void print_words(WINDOW *win, bool found) {
    BoardWord *p = legal;
    int i = 0;
    int rows = wwords_row - 2;
    while (p != NULL) {
        if (p->found == found) {
            // mvwprintw(win, i / 5 + 1, (i % 5) * 15 + 2, "%s\n", p->word);
            mvwprintw(win, i % rows + 1, (i / rows) * 15 + 2, "%s\n", p->word);
            i++;
        }
        p = p->next;
    }
    box(win, 0, 0);
    wrefresh(win);
}

/** Find all words */

void check_all() {
    FILE *dict = fopen(WORDS_PATH, "r");
    char *word = NULL;
    size_t bufsize = 0;
    ssize_t nread;
    while ((nread = getline(&word, &bufsize, dict)) > 0) {
        if (nread < 4 || isupper(word[0]))
            continue;
        word[nread - 1] = '\0'; // trim newline
        if (find_word(word))
            add_word(word);
    }
    free(word);
}

/** Player guesses word
 *
 * 0 = not a word
 * -1 already guessed
 * 1 = ok
 */

int guess_word(char word[]) {
    BoardWord *p = legal;
    while (p != NULL) {
        if (strcmp(p->word, word) == 0) {
            if (p->found)
                return -1;
            p->found = 1;
            return 1;
        }
        p = p->next;
    }
    return 0;
}

#if 0
int main() {
  make_board();
  display_board();
  check_all();
  char *word = NULL;
  size_t bufsize = 0;
  int nread;
  printf("\nWord? (control-D quits) ");
  while ((nread = getline(&word, &bufsize, stdin)) > 0) {
    word[nread - 1] = '\0';
    printf("     %s: %d\n", word, guess_word(word));
    printf("\nAnother? ");
  }
  printf("\n\nFound:\n");
  print_words(true);
  printf("\nMissed:\n");
  print_words(false);
}
#endif
