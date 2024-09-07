#include <complex.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <stdlib.h>

#define MAX_CLIENTS 256
#define PORT 8080
#define BUFF_SIZE 4096

typedef enum {
  STATE_NEW,
  STATE_CONNECTED,
  STATE_DISCONNECTED,
} state_e;

// Structure to hold client state
typedef struct {
  int fd;
  state_e state;
  char buffer[BUFF_SIZE];
} clientstate_t;

clientstate_t client_states[MAX_CLIENTS];

void init_clients() {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    client_states[i].fd = -1; // -1 indicates a free slot
    client_states[i].state = STATE_NEW;
    memset(&client_states[i].buffer, '\0', BUFF_SIZE);
  }
}

int find_free_slot() {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (client_states[i].fd == -1) {
      return i;
    }
  }
  return -1; // No free slot found
}

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
  int real_length = hdr->length;
  hdr->length = htons(hdr->length);

  int *data = (int*) &hdr[1]; // Pointer magic
  *data = htonl(1); // Protocol version 1

  write(fd, hdr, sizeof(proto_hdr_t) + real_length);
}

int main(int argc, char *argv[]) {
  int listen_fd, connected_fd, nfds, free_slot;
  struct sockaddr_in server_address, client_address;
  socklen_t client_length = sizeof(client_address);
  fd_set read_fds, write_fds;

  // Initialise client states
  init_clients();

  // Create listening socket
  if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // Set up server address structure
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(PORT);

  // Bind
  if (bind(listen_fd, (struct sockaddr*) &server_address, sizeof(struct sockaddr)) == -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  // Listen
  if (listen(listen_fd, 10) == -1) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  printf("Server listening on port %d\n", PORT);

  while (1) {
    // Clear the FD sets
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);

    // Add the listening socket to the read set
    FD_SET(listen_fd, &read_fds);
    nfds = listen_fd + 1;

    // Add active connections to the read set
    for (int i = 0; i < MAX_CLIENTS; i++) {
      if (client_states[i].fd != -1) {
        FD_SET(client_states[i].fd, &read_fds);
        if (client_states[i].fd >= nfds) {
          nfds = client_states[i].fd + 1;
        }
      }
    }

    // Wait for an activity on one of the sockets
    if (select(nfds, &read_fds, &write_fds, NULL, NULL) == -1) {
      perror("select");
      exit(EXIT_FAILURE);
    }

    // Check for new connections
    if (FD_ISSET(listen_fd, &read_fds)) {
      if ((connected_fd = accept (listen_fd, (struct sockaddr *) &client_address, &client_length)) == -1) {
        perror("accept");
        continue;
      }

      printf("New connection from %s:%d\n",
          inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

      // Find a free slot for the new connection
      free_slot = find_free_slot();
      if (free_slot == -1) {
        printf("Server full: closing new connection\n");
        close(connected_fd);
      } else {
        client_states[free_slot].fd = connected_fd;
        client_states[free_slot].state = STATE_CONNECTED;
      }
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
      if (client_states[i].fd != -1 && FD_ISSET(client_states[i].fd, &read_fds)) {
        // Read
        ssize_t bytes_read = read(client_states[i].fd, &client_states[i].buffer, BUFF_SIZE);

        if (bytes_read <= 0) {
          close(client_states[i].fd);
          client_states[i].fd = -1;
          client_states[i].state = STATE_DISCONNECTED;
          memset(&client_states[i].buffer, '\0', BUFF_SIZE);
          printf("Client disconnected or error \n");
        } else {
          printf("Received data from client: %s\n", client_states[i].buffer);
        }
      }
    }

  } // End of while(1)

  return 0;

}
