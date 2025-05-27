/** cboggle: a straightforward boggle game using ncurses and a DAWG. */

#include <stdint.h>
#include <err.h>

#ifndef BOGGLE_H
#define BOGGLE_H

// Given that we use a 64-bit mask to keep track of tiles used, the board
// cannot have more than 64 spots. Non-square boards are unconventional but
// should work.
#define HEIGHT 4
#define WIDTH 4
#define FATAL2(m, m2) \
  err(1, "%s:%i: %s: %s %s", __FILE__, __LINE__, __func__, m, m2)

typedef struct {
    const char *word;
    bool found;
} BoardWord;

extern BoardWord *legal;
extern int board_nwords;
extern int board_longest;
extern int board_score;
extern int player_nwords;
extern int player_score;

#endif /* end of boggle_h */
