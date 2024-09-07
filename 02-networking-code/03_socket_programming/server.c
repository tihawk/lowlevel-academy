#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

typedef enum {
  PROTO_HELLO,
} proto_type_e;

// TLV - type length value
typedef struct {
  proto_type_e type;
  unsigned short length;
} proto_hdr_t;

void handle_client(int fd) {
  char buf[4096] = {0};
  proto_hdr_t *hdr = buf;
  hdr->type = htonl(PROTO_HELLO);
  hdr->length = sizeof(int);
  int realLength = hdr->length;
  hdr->length = htons(hdr->length);

  int *data = (int*) &hdr[1]; // Pointer magic
  *data = htonl(1); // Protocol version 1

  write(fd, hdr, sizeof(proto_hdr_t) + realLength);
}

int main(int argc, char *argv[]) {
  struct sockaddr_in clientInfo = {0};
  unsigned int clientSize = 0;
  struct sockaddr_in serverInfo = {0};
  serverInfo.sin_family = AF_INET;
  serverInfo.sin_addr.s_addr = 0;
  serverInfo.sin_port = htons(5555);

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("socket");
    return -1;
  }

  // Bind
  if (bind(fd, (struct sockaddr*) &serverInfo, sizeof(struct sockaddr)) == -1) {
    perror("bind");
    close(fd);
    return -1;
  }
  // Listen
  if (listen(fd, 0) == -1) {
    perror("listen");
    close(fd);
    return -1;
  }
  while (1) {
    // Accept
    int cfd = accept(fd, (struct sockaddr*) &clientInfo, &clientSize);
    if (cfd == -1) {
      perror("accept");
      close(fd);
      return -1;
    }

    handle_client(cfd);

    close(cfd);
  }

}
