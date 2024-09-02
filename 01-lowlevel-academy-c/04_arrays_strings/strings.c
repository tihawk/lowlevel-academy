#include <stdio.h>

int main() {
  // Strings in C are null-terminated, have to end on 0
  char myStr[] = {'H','e','l','l','o', 0};

  char *myOtherStr = "World!";

  //strcpy(dest, src);
  //strncpy(dest, src, n);

  printf("%s %s\n", myStr, myOtherStr);
}
