#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <filename>\n", argv[0]);
    return 0;
  }
  // The O_RDWR, O_CREAT and other such flags are bitwise, so we can OR them
  // using the <|> operator to combine them.
  // The last argument is the permissions to be set for the file.
  int fd = open(argv[1], O_RDWR | O_CREAT, 0644);
  if (fd == -1) {
    perror("open");
    return -1;
  }

  char *myData = "Hello there, file!\n";

  write(fd, myData, strlen(myData));
  close(fd);
  return 0;
}
