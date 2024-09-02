#include <stdio.h>

// Compilation process
// - Preprocessor
// - Compilation
// - Assembling
// - Linking

#define MAX_PERSONS 1024

// DEBUG mode can be enabled from gcc
// gcc -o preprocessor preprocessor.c -DDEBUG
#define DEBUG

int main() {
  #ifdef DEBUG
  printf("WE ARE IN DEBUG MODE: %s:%d", __FILE__, __LINE__);
  #endif

  return 0;
}
