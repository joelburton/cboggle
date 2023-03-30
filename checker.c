#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include "check.h"

int check_word(const int32_t *restrict dawg, const char *word) {
    int32_t i = 1;

    while (true) {
        if (DAWG_LETTER(dawg, i) == *word) {
            // got matching letter -- are we at the end of sought word?
            if (*++word == '\0') {
                // yes, so end of search -- if this is valid word-end, win!
                return DAWG_EOW(dawg, i);
            } else {
                // not at end of word, so proceed to next letter
                i = DAWG_CHILD(dawg, i);
            }
        } else {
            // not the right word, so scan to next sibling
            i = DAWG_NEXT(dawg, i);
        }
        // either next sibling or next child could be no-such-letter, lose
        if (!i) return -1;
    }
}

_Noreturn void fatal(char *func) {
    perror(func);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "No words provided");
        exit(1);
    }

    const int fd = open("words.dat", O_RDONLY);
    if (fd < 0) fatal("Cannot open " "words.dat");

    int32_t nelems;
    if (read(fd, &nelems, 4) < 4) fatal("Cannot get file size");

    int32_t *f = mmap(NULL, (size_t) nelems * 4, PROT_READ, MAP_PRIVATE, fd, 0);
    if (f == MAP_FAILED) fatal("Cannot read dictionary");

    const int32_t *restrict dawg = f + 1;

    for (int i = 1; i < argc; i++) {
        char *w = argv[i];
        for (int j = 0; w[j]; j++) w[j] = (char) toupper(w[j]);
        printf("%s = %d\n", w, check_word(dawg,  w));
    }
}
