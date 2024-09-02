#include <stdio.h>

// Creates space for its biggest element
// like int in this case
union myUnion_u {
  int x;
  char c;
  short s;
};

int main() {
  union myUnion_u u;
  u.x = 0x41424344;

  // We are reading the lower bytes of the int
  // by reading the values of the smaller types, short and char
  // where short will show the 4 least significant bytes,
  // and char will show the 2 least significant bytes.
  // Black magic, really.
  printf("%hx %hhx\n", u.s, u.c);
}
