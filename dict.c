#include <stdlib.h>
#include <assert.h>
#include "boggle.h"


/** Add word to trie of words in dict. */

static void trie_add(char *word) {
  Trie *lt = trie;

  while (*word != '\0') {
    int ltr = word[0] - 'a';
    assert (ltr >= 0 && ltr < 26);

    if (lt->letters[ltr] == NULL)
      lt->letters[ltr] = calloc(1, sizeof(Trie));

    lt = lt->letters[ltr];
    word++;
  }

  // We're at the end of the word, so mark this trie node as winning
  lt->term = true;
}

/** Read dictionary file.
 *
 * File must be a all lowercase, already pruned to word at least 3-chars long,
 * and without any punctuation.
 */

void read_all() {
  // Since we use a trie to hold the words, create it.
  trie = calloc(1, sizeof(Trie));

  FILE *dict = fopen(WORDS_PATH, "r");
  if (dict == NULL) {
    perror(__func__);
    exit(EXIT_FAILURE);
  }

  char *word = NULL;
  size_t bufsize = 0;
  ssize_t nread;

  while ((nread = getline(&word, &bufsize, dict)) > 0) {
    word[nread - 1] = '\0'; // trim newline
    trie_add(word);
  }

  free(word);
}