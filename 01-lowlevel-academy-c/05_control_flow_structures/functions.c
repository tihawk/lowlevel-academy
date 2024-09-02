#include <stdio.h>

// return_type name(list of arguments) {
//
// }

int add(int x, int y) {
  int z = x + y;

  return z;
}

int main(int argc, char *argv[]) {

  int otherInt = add(1, 2);
  printf("%d\n", otherInt);

  return 0;
}
