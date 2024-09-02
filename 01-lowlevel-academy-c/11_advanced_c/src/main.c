#include <stdio.h>

#include "../include/file.h"
#include "../include/parse.h"

int main(int argc, char *argv[]) {
  int fd, numEmployees = 0;

  if (argc != 2) {
    printf("Usage: %s <filename>\n", argv[0]);
    return 0;
  }

  fd = open_file_rw(argv[1]);
  if (fd == -1) return -1;

  if (parse_file_header(fd, &numEmployees)) return -1;

  printf("Number of employees stored: %d\n", numEmployees);

  return 0;
}
