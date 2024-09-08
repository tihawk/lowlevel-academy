#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>

#include "../../include/common.h"

int send_hello(int fd) {
  char buf[BUFF_SIZE] = {0};

  dbproto_header_t *proto_header = buf;
  proto_header->type = MSG_HELLO_REQ;
  proto_header->length = 1;
  
  dbproto_hello_req *hello_req = (dbproto_hello_req*) &proto_header[1]; // I still hate this
  hello_req->proto = PROTO_VERSION;

  proto_header->type = htonl(proto_header->type);
  proto_header->length = htons(proto_header->length);
  hello_req->proto = htons(hello_req->proto);

  write(fd, buf, sizeof(dbproto_header_t) + sizeof(dbproto_hello_req));

  read(fd, buf, sizeof(buf));

  proto_header->type = ntohl(proto_header->type);
  proto_header->length = ntohs(proto_header->length);

  if (proto_header->type == MSG_ERROR) {
    printf("Protocol mismatch...\n");
    return STATUS_ERROR;
  }

  dbproto_hello_resp *hello_res = (dbproto_hello_resp*) &proto_header[1]; // And this is even worse

  printf("Server connected, protocol v%d\n", ntohs(hello_res->proto));
  return STATUS_SUCCESS;
}

int send_employee(int fd, char *employee_str) {
  char buf[BUFF_SIZE] = {0};

  dbproto_header_t *proto_header = buf;
  proto_header->type = MSG_EMPLOYEE_ADD_REQ;
  proto_header->length = 1;

  dbproto_employee_add_req *employee_add_req =
    (dbproto_employee_add_req *) &proto_header[1];
  strncpy(&employee_add_req->data, employee_str, sizeof(employee_add_req->data));

  proto_header->type = htonl(proto_header->type);
  proto_header->length = htons(proto_header->length);

  write(fd, buf, sizeof(dbproto_header_t) + sizeof(dbproto_employee_add_req));

  read(fd, buf, sizeof(buf));

  proto_header->type = ntohl(proto_header->type);
  proto_header->length = ntohs(proto_header->length);

  if (proto_header->type == MSG_ERROR) {
    printf("Improper format or data\n");
    return STATUS_ERROR;
  }

  printf("Succesfully added employee\n");
  return STATUS_SUCCESS;
}

int main(int argc, char *argv[]) {
  char *add_arg = NULL;
  char *port_arg = NULL;
  char *host_arg = NULL;
  int c;
  
  unsigned short port = 0;

  while ((c = getopt(argc, argv, "p:h:a:")) != -1) {
    switch (c) {
      case 'a':
        add_arg = optarg;
        break;
      case 'p':
        port_arg = optarg;
        port = atoi(port_arg);
        break;
      case 'h':
        host_arg = optarg;
        break;
      case '?':
        printf("Unknown option -%c\n", c);
      default:
        return -1;
    }
  }

  if (port == 0) {
    printf("Bad port: %s\n", port_arg);
    return -1;
  }

  if (host_arg == NULL) {
    printf("Must specify a host with -h\n");
  }

  struct sockaddr_in serverInfo = {0};
  serverInfo.sin_family = AF_INET;
  serverInfo.sin_addr.s_addr = inet_addr(host_arg);
  serverInfo.sin_port = htons(port);

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("socket");
    return -1;
  }

  if (connect(fd, (struct sockaddr*) &serverInfo, sizeof(struct sockaddr)) == -1 ) {
    perror("connect");
    close(fd);
    return 0;
  }
  
  if (send_hello(fd) != STATUS_SUCCESS) {
    close(fd);
    return -1;
  }

  send_employee(fd, add_arg);

  close(fd);
}
