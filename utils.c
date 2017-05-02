#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/** Strip whitespace from string. */

char *strip_whitespace(char *str) {
  while (*str == ' ')
    str++;

  for (int i = strlen(str) - 1; i > 0 && str[i] == ' '; i--)
    str[i] = '\0';

  return str;
}

_Noreturn void fatal(const char *func) {
  perror(func);
  exit(EXIT_FAILURE);
}
