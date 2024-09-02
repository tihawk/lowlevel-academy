#include <stdio.h>
#include <stdlib.h>

typedef enum {
  STATUS_BAD,
  STATUS_GOOD
} status_t;

// Double pointers are used to pass pointers as reference.
// Remember that passing by reference in C is implemented by passing pointers
// into the functions, to be dereferenced by them.

status_t foo(int **data, int len) {

  int *temp = NULL;
  temp = realloc(*data, len);

  if (temp == NULL) {
    *data = NULL;
    return STATUS_BAD;
  }

  *data = temp;
  return STATUS_GOOD;
}

int main() {
  int *first = malloc(64);
  if (STATUS_BAD == foo(&first, 128)) {
    printf("Uh oh\n");
  }

  return 0;
}
