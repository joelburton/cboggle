#include <time.h>
#include "boggle.h"

int main() {
    clock_t start;

    start = clock();
    read_all();
    printf("read sec = %f\n", (clock() - start) / (double) CLOCKS_PER_SEC);

    start = clock();
    find_all_words();
    printf("check sec = %f\n", (clock() - start) / (double) CLOCKS_PER_SEC);


  }
