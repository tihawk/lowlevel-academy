#include <stdio.h>

#define MAX_IDS 32

int main() {
  //int ids[MAX_IDS];

  int ids[MAX_IDS] = {0, 1, 2};
  
  //int ids[] = { 1, 2, 3 };

  printf("%d\n", ids[0]);

  ids[3] = 0x41;
  printf("%d\n", ids[3]);
}
