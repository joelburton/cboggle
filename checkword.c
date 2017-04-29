#include <time.h>
#include "boggle.h"

int main() {
    clock_t start = clock();
    check_all();
    printf("sec = %f\n", (clock() - start) / (double) CLOCKS_PER_SEC);
  }
