#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#define CHILD_BIT_SHIFT 10
#define EOW_BIT_MASK 0X00000200
#define EOL_BIT_MASK 0X00000100
#define LTR_BIT_MASK 0X000000FF

#define DAWG_LETTER(arr, i) (arr[i] & LTR_BIT_MASK)
#define DAWG_EOW(arr, i)    (arr[i] & EOW_BIT_MASK)
#define DAWG_NEXT(arr, i)  ((arr[i] & EOL_BIT_MASK) ? 0 : i + 1)
#define DAWG_CHILD(arr, i)  (arr[i] >> CHILD_BIT_SHIFT)


int check_word(const int32_t *restrict dawg, const char *word) {
    int_fast32_t i = 1;

    while (true) {

        if (DAWG_LETTER(dawg, i) == *word)
            // got matching letter -- are we at the end of sought word?
            if (*++word == '\0')
                // yes, so end of search -- if this is a valid word-end, we win!
                return DAWG_EOW(dawg, i);

            else
                // not at end of word, so proceed to next letter
                i = DAWG_CHILD(dawg, i);

        else
            // not the right word, so scan to next sibling
            i = DAWG_NEXT(dawg, i);

        // either the next sibling or the next child could be no-such-letter, lose
        if (!i)
            return -1;
    }
}

_Noreturn void fatal(char *func) {
    perror(func);
    exit(EXIT_FAILURE);
}

int main() {
    clock_t start = clock();

    const int fd = open("words.dat", O_RDONLY);
    if (fd < 0)
        fatal("Cannot open " "words.dat");

    int32_t nelems;
    if (read(fd, &nelems, 4) < 4)
        fatal("Cannot get file size");

    int32_t *f = mmap(NULL, (size_t) nelems * 4, PROT_READ, MAP_PRIVATE, fd, 0);
    if (f == MAP_FAILED)
        fatal("Cannot read dictionary");

    const int32_t *restrict dawg = f + 1;

    printf("%f\n\n", (clock() - start) / (double) CLOCKS_PER_SEC);

    char *words[] =
            {"AA", "AD", "AC", "APPLE", "APPLES", "APPPLES", "APPL", "APPLESAUCES",
             "BANANA"};
    for (int i = 0; i < sizeof(words) / sizeof(char *); i++) {
        start = clock();
        printf("%s = %d\n", words[i], check_word(dawg, words[i]));
        printf("%f\n\n", (clock() - start) / (double) CLOCKS_PER_SEC);
    }

}