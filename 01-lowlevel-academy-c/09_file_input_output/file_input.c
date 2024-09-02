#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// Run this to create an example database header file
// echo -ne "\x01\x00\xff\x00\x08\x00\x00\x00" > ./my-db.db
struct database_header_t {
  unsigned short version;
  unsigned short employees;
  unsigned int fileSize;
};

int main(int argc, char *argv[]) {
  struct database_header_t header = {0};

  if (argc != 2) {
    printf("Usage: %s <filename>\n", argv[0]);
    return 0;
  }
  // The O_RDWR, O_CREAT and other such flags are bitwise, so we can OR them
  // using the <|> operator to combine them.
  // The last argument is the permissions to be set for the file.
  int fd = open(argv[1], O_RDONLY);
  if (fd == -1) {
    perror("open");
    return -1;
  }

  if (read(fd, &header, sizeof(header)) != sizeof(header)) {
    perror("read");
    close(fd);
    return -1;
  }

  printf("DB Version: %u\n", header.version);
  printf("DB Number of Employees: %u\n", header.employees);
  printf("DB File Length: %u\n", header.fileSize);

  close(fd);
  return 0;
}
