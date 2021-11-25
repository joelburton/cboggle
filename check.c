#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "boggle.h"

/** DAWG handling. */

#define CHILD_BIT_SHIFT 10
#define EOW_BIT_MASK 0X00000200
#define EOL_BIT_MASK 0X00000100
#define LTR_BIT_MASK 0X000000FF

#define DAWG_LETTER(arr, i) (arr[i] & LTR_BIT_MASK)
#define DAWG_EOW(arr, i)    (arr[i] & EOW_BIT_MASK)
#define DAWG_NEXT(arr, i)  ((arr[i] & EOL_BIT_MASK) ? 0 : i + 1)
#define DAWG_CHILD(arr, i)  (arr[i] >> CHILD_BIT_SHIFT)

/** Scoring */

static const int WORD_SCORES[] = {
        0, 0, 0, 1, 1, 2, 3, 5, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11};

/** Compare board words by the actual word. */

static int boardwords_cmp(const void *a,
                          const void *b) {
    const BoardWord *aa = a;
    const BoardWord *bb = b;
    return strcmp(aa->word, bb->word);
}

/** Add word to tree of legal words. Returns T/F if it is new. */

static bool add_word(const char word[], int length) {
    BoardWord *new_word = malloc(sizeof(BoardWord));
    new_word->word = word;
    new_word->found = false;

    BoardWord **newNode = tsearch(new_word, (void **) &legal, boardwords_cmp);
    if (new_word == *newNode) {
        // newNode is a new word
        new_word->word = strdup(word);   // know we'll keep it, so strdup it
        board_nwords++;
        board_score += WORD_SCORES[strlen(word)];
        if (length > board_longest)
            board_longest = length;
        return true;
    } else {
        free(new_word);
        return false;
    }
}

/** Find all words starting from this tile and DAWG-pointer.
 *
 * This is a recursive function -- it is given a tile (via y and x)
 * and a DAWG pointer of where it is in a current word (along with the word
 * and word_len for that word). For example, it might be given the tile at
 * (1,1) and a DAWG-pointer to the end letter of C->A->T. For this example,
 * word="CAT" and word_len=3. It would the note that "CAT" is a good word,
 * and the recurse to all the neighboring tiles.
 *
 * Since you can only use a given tile once per word, it keeps a bitmask of
 * used tile positions. If the tile at the given position is already used,
 * this returns without continuing searching.
 *
 * @param i          Pointer to item in DAWG
 * @param word       Word that we're currently making
 * @param word_len   length of word we're currently making
 * @param y          y pos of tile
 * @param x          x pos of tile
 * @param used       bitmask of tile positions used
 */

static void find_words(
        unsigned int i, char *word, int word_len, int y, int x, int_least64_t used) {

    // If not a legal tile, can't make word here
    if (y < 0 || y >= HEIGHT || x < 0 || x >= WIDTH)
        return;

    // Make bitmask for this tile position
    int_least64_t mask = 0x1 << (y * WIDTH + x);

    // If we've already used this tile, can't make word here
    if (used & mask)
        return;

    // Find the DAWG-node for existing-DAWG-node plus this letter.
    char sought = (char) toupper(board[y][x]);

    while (i != 0 && DAWG_LETTER(dawg, i) != sought)
        i = DAWG_NEXT(dawg, i);

    if (i == 0)
        // There are no words continuing with this letter
        return;

    // Mark tile as used
    used |= mask;

    // Either this is a word, or the stem of a word. So update our 'word' to
    // include this letter.
    word[word_len++] = (char) tolower(board[y][x]);

    // Add this word to the found-words.
    if (DAWG_EOW(dawg, i)) {
        word[word_len] = '\0';
        add_word(word, word_len);
    }

    // Check every direction H/V/D from here (will also re-check this tile, but
    // the can't-reuse-this-tile rule prevents it from actually succeeding)
    for (int di = -1; di < 2; di++)
        for (int dj = -1; dj < 2; dj++)
            find_words(DAWG_CHILD(dawg, i), word, word_len, y + di, x + dj, used);
}

/** Free word inside a BoardWord. Called by g_sequence_free. */


void delNode(
        const void *nodep, VISIT value, int level __attribute__((unused))) {
    const BoardWord *n = *(const BoardWord **) nodep;

    if (value == leaf || value == endorder) {
        free((void *) n->word);
        free((void *) n);
        free((void *) nodep);
    }
}


/** Find all words on board. */

void find_all_words() {
    char *const word = alloca(17);

    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            find_words(1, word, 0, i, j, 0x0);
}

/** Free list of legal words. */

void free_words() {
    twalk(legal, delNode);
    legal = NULL;
}

/** Check player guess.
 *
 * @return GUESS_BAD, GUESS_DUP, or GUESS_GOOD
 */

int guess_word(char *word) {
    BoardWord bw = {.word = word};
    BoardWord **found = tfind(&bw, (void **) &legal, boardwords_cmp);

    if (found == NULL) return GUESS_BAD;

    // "found" is pointer to tree node (which is pointer to BoardWord);
    if ((*found)->found) return GUESS_DUP;

    (*found)->found = true;
    player_nwords++;
    player_score += WORD_SCORES[strlen(word)];
    return GUESS_GOOD;
}
