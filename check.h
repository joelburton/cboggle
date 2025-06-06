#ifndef BOGGLE_CHECK_H
#define BOGGLE_CHECK_H

/** DAWG handling. */

#define CHILD_BIT_SHIFT 10
#define EOW_BIT_MASK 0X00000200
#define EOL_BIT_MASK 0X00000100
#define LTR_BIT_MASK 0X000000FF

#define DAWG_LETTER(arr, i) ((arr)[i] & LTR_BIT_MASK)
#define DAWG_EOW(arr, i)    ((arr)[i] & EOW_BIT_MASK)
#define DAWG_NEXT(arr, i)  (((arr)[i] & EOL_BIT_MASK) ? 0 : (i) + 1)
#define DAWG_CHILD(arr, i)  ((arr)[i] >> CHILD_BIT_SHIFT)

#define GUESS_GOOD 1
#define GUESS_BAD 0
#define GUESS_DUP (-1)

extern const int32_t *dawg;

void find_all_words();

void free_words();

int guess_word(char word[]);

void read_dawg(bool local);

#endif //BOGGLE_CHECK_H
