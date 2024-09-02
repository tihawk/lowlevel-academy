#include <stdio.h>
#include <unistd.h>

#include "../include/common.h"
#include "../include/parse.h"

int parse_file_header(int fd, int *numEmployeesOut) {
  if (fd == -1) {
    printf("Bad file descriptor provided\n");
    return STATUS_ERROR;
  }

  struct dbHeader_t header = {0};
  if (read(fd, &header, sizeof(header)) != sizeof(header)) {
    printf("Failed to read file header\n");
    return STATUS_ERROR;
  }

  *numEmployeesOut = header.count;
  return STATUS_SUCCESS;
}
