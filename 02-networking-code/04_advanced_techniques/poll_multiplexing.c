#include <asm-generic/socket.h>
#include <complex.h>
#include <stdio.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <stdlib.h>
#include <poll.h>
#include <sys/time.h>

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

int find_slot_by_fd(int fd) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (client_states[i].fd == fd) {
      return i;
    }
  }
  return -1; // Not found
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
  int listen_fd, connected_fd, free_slot;
  struct sockaddr_in server_address, client_address;
  socklen_t client_length = sizeof(client_address);

  struct pollfd fds[MAX_CLIENTS + 1];
  int nfds = 1;
  int opt = 1;

  // Initialise client states
  init_clients();

  // Create listening socket
  if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // Make socket non-waiting
  // This will forcibly use the address if it's already bound
  // For example if the server crashes
  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
    perror("setsockopt");
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

  memset(fds, 0, sizeof(fds));
  fds[0].fd = listen_fd;
  fds[0].events = POLLIN;
  nfds = 1;

  while (1) {
    
    int fdsI = 1;
    for (int i = 0; i < MAX_CLIENTS; i++) {
      if (client_states[i].fd != -1) {
        fds[fdsI].fd = client_states[i].fd; // Offset by 1 for listen_fd
        fds[fdsI].events = POLLIN;
        fdsI++;
      }
    }


    // Wait for an event on one of the sockets
    int n_events = poll(fds, nfds, -1); // -1 means no timeout
    if (n_events == -1) {
      perror("poll");
      exit(EXIT_FAILURE);
    }

    // Check for new connections
    if (fds[0].revents & POLLIN) {
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
        nfds++;
        printf("Slot %d has fd %d\n", free_slot, client_states[free_slot].fd);
      }
      n_events--;
    }

    for (int i = 1; i <= nfds && n_events > 0; i++) {
      if (fds[i].revents & POLLIN) {
        n_events--;

        int fd = fds[i].fd;
        int slot = find_slot_by_fd(fd);

        // Read
        ssize_t bytes_read = read(client_states[slot].fd, &client_states[slot].buffer, BUFF_SIZE);
        if (bytes_read <= 0) {
          close(fd);
          if (slot == -1) {
            printf("Tried to close fd that doesn't exit?\n");
          } else {
            client_states[slot].fd = -1;
            client_states[slot].state = STATE_DISCONNECTED;
            memset(&client_states[slot].buffer, '\0', BUFF_SIZE);
            printf("Client disconnected or error \n");
            nfds--;
          }
        } else {
          printf("Received data from client: %s\n", client_states[slot].buffer);
        }
      }
    }

  } // End of while(1)

  return 0;

}
