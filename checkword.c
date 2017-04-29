


#if 0
  int main() {
    make_board();
    display_board();
    printf("\nWord? ");
    char word[NTILES + 1];
    scanf("%" NTILES_STR "s", word);
    bool found = find_word(word);
    printf("%sfound\n", found ? "" : "not ");
  }
#endif
