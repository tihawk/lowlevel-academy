#include <stdio.h>

int main(int argc, char *argv[]) {
  // char 1 byte, 8 bits
  // short 2 bytes, 16 bits
  // int 4 bytes, 32 bits
  // long long int 8 byte, 64 bits
  // unsigned int xx = -4;
  // float f 32 bit float point
  // double 64 bit float point

  // same size
  unsigned int x = 4;
  int sx = (int)(x); // same size, but different type

  // downcasting
  unsigned int bigX = 0xfffffefe;
  short kindaBig = (short)bigX; // getting only the least significant 4 bytes 0xfefe

  // upcasting
  short smallX = -1; // 0xffff
  int wasAShort = (int)smallX; // sign extension. instead of 0x0000ffff we get 0xffffffff

  // crosscasting
  float f = 3.14;
  int wasAFloat = (int)f; // rounds it to the nearest integer, i.e. 3
  
  //(char *)
  //(short *)

  short *mydata = (short*)argv[1]; // casts to a short pointer

  return 0;
}
