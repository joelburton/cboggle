#include <time.h>
#include <stdlib.h>
#include "boggle.h"

_Noreturn void fatal(const char *func) {
  perror(func);
  exit(1);
}

int main() {
    clock_t start;

    start = clock();
    read_all();
    printf("read sec = %f\n", (clock() - start) / (double) CLOCKS_PER_SEC);

    start = clock();
    find_all_words();
    printf("check sec = %f\n", (clock() - start) / (double) CLOCKS_PER_SEC);


  }
