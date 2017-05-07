#include "boggle.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

/** Compare a BoardWord to a given word.
 *
 * Does a comparison of a BoardWord to a word; used to decide to add a word.
 **/

static gint boardwords_cmp_w(gconstpointer a,
                             gconstpointer b,
                             gpointer data __attribute__ ((unused))) {
  const char *aa = ((BoardWord *) a)->word;
  const char *bb = b;
  return strcmp(aa, bb);
}

/** Compare boardwords by the actual word. */

static gint boardwords_cmp(gconstpointer a,
                           gconstpointer b,
                           gpointer data __attribute__ ((unused))) {
  const char *aa = ((BoardWord *) a) -> word;
  const char *bb = ((BoardWord *) b) -> word;
  return strcmp(aa, bb);
}

/** Add word to linked list of legal words. */

static bool add_word(const char word[]) {
  if (g_sequence_lookup(legal, (gpointer) word, boardwords_cmp_w, NULL))
    return false;

  BoardWord *new_word = malloc(sizeof(BoardWord));
  new_word->word = strdup(word);
  new_word->found = false;
  board_nwords++;
  board_score += WORD_SCORES[strlen(word)];

  g_sequence_insert_sorted(legal, new_word, boardwords_cmp, NULL);
  return true;
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
  char sought = toupper(board[y][x]);

  while (i != 0 && DAWG_LETTER(dawg, i) != sought)
    i = DAWG_NEXT(dawg, i);

  if (i == 0)
    // There are no words continuing with this letter
    return;

  // Mark tile as used
  used |= mask;

  // Either this is a word, or the stem of a word. So update our 'word' to
  // include this letter.
  word[word_len++] = tolower(board[y][x]);

  // Add this word to the found-words.
  if (DAWG_EOW(dawg, i)) {
    word[word_len] = '\0';
    add_word(word);
  }

  // Check every direction H/V/D from here (will also re-check this tile, but
  // the can't-reuse-this-tile rule prevents it from actually succeeding)
  for (int di = -1; di < 2; di++)
    for (int dj = -1; dj < 2; dj++)
      find_words(DAWG_CHILD(dawg, i), word, word_len, y + di, x + dj, used);
}

/** Free word inside a boardword. Called by g_sequence_free. */

static void boardwords_free(gpointer bw) {
  free((void *) ((BoardWord *) bw)->word);
}

/** Find all words on board. */

void find_all_words() {
  legal = g_sequence_new(boardwords_free);
  char *const word = malloc(17);

  for (int i = 0; i < HEIGHT; i++)
    for (int j = 0; j < WIDTH; j++)
      find_words(1, word, 0, i, j, 0x0);
}

/** Free list of legal words. */

void free_words() {
  g_sequence_free(legal);
}

/** Check player guess.
 *
 * @return 0 (not a word), -1 (already guessed), or 1 (good)
 */

int guess_word(char word[]) {
  GSequenceIter *iter = g_sequence_lookup(
      legal, (gpointer) word, boardwords_cmp_w, NULL);

  if (iter == NULL) return 0;

  BoardWord *w = g_sequence_get(iter);

  if (w->found) return -1;

  w->found = true;
  player_nwords++;
  player_score += WORD_SCORES[strlen(word)];
  return 1;
}
