#include <arpa/inet.h>
#include <bits/getopt_core.h>
#include <netinet/in.h>
#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../../include/common.h"
#include "../../include/file.h"
#include "../../include/parse.h"
#include "../../include/server_poll.h"

clientstate_t client_states[MAX_CLIENTS] = {0};

void print_usage(char *argv[]) {
  printf("Usage: %s -n -f <database file>\n", argv[0]);
  printf("\t -n  - create new database file\n");
  printf("\t -f  - (required) path to database file\n");
  printf("\t -p  - (required) port to listen to\n");
  return;
}

void poll_loop(unsigned short port, struct db_header_t *dbheader,
    struct employee_t *employees, int dbfd) {
  int listen_fd, connected_fd, free_slot;
  struct sockaddr_in server_address, client_address;
  socklen_t client_length = sizeof(client_address);

  struct pollfd fds[MAX_CLIENTS + 1];
  int nfds = 1;
  int opt = 1;

  // Initialise client states
  init_clients(client_states);

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
  server_address.sin_port = htons(port);

  // Bind
  if (bind(listen_fd, (struct sockaddr*) &server_address,
        sizeof(struct sockaddr)) == -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  // Listen
  if (listen(listen_fd, 10) == -1) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  printf("Server listening on port %d\n", port);

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
      if ((connected_fd = accept (listen_fd,
              (struct sockaddr *) &client_address, &client_length)) == -1) {
        perror("accept");
        continue;
      }

      printf("New connection from %s:%d\n",
          inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

      // Find a free slot for the new connection
      free_slot = find_free_slot(client_states);
      if (free_slot == -1) {
        printf("Server full: closing new connection\n");
        close(connected_fd);
      } else {
        client_states[free_slot].fd = connected_fd;
        client_states[free_slot].state = STATE_HELLO;
        nfds++;
        printf("Slot %d has fd %d\n", free_slot, client_states[free_slot].fd);
      }
      n_events--;
    }

    for (int i = 1; i <= nfds && n_events > 0; i++) {
      if (fds[i].revents & POLLIN) {
        n_events--;

        int fd = fds[i].fd;
        int slot = find_slot_by_fd(client_states, fd);

        // Read
        ssize_t bytes_read = read(client_states[slot].fd,
            &client_states[slot].buffer, BUFF_SIZE);
        if (bytes_read <= 0) {
          close(fd);
          if (slot == -1) {
            printf("Tried to close fd that doesn't exit?\n");
          } else {
            client_states[slot].fd = -1;
            client_states[slot].state = STATE_DISCONNECTED;
            memset(&client_states[slot].buffer, '\0', BUFF_SIZE);
            printf("Client disconnected\n");
            nfds--;
          }
        } else {
          printf("Received data from client: %s\n", client_states[slot].buffer);
          handle_client_fsm(dbheader, &employees, &client_states[slot], dbfd);
        }
      }
    }

  } // End of while(1)

  return;

}

int main (int argc, char *argv[]) {
  
  char *port_arg = NULL;
  char *add_string_arg = NULL;
  char *filepath_arg = NULL;
  bool list_arg = false;
  bool newfile_arg = false;
  int c;

  unsigned short port = 0;

  // DB file descriptor
  int dbfd = -1;
  // DB header
  struct db_header_t *header = NULL;
  struct employee_t *employees = NULL; // Apparently it doesn't matter that it's an array of employees

  while ((c = getopt(argc, argv, "nf:a:lp:")) != -1) {
    switch (c) {
      case 'n':
        newfile_arg = true;
        break;
      case 'f':
        filepath_arg = optarg;
        break;
      case 'l':
        list_arg = true;
        break;
      case 'p':
        port_arg = optarg;
        port = atoi(port_arg);
        if (port == 0) {
          printf("Bad port: %s\n", port_arg);
        }
        break;
      case 'a':
        add_string_arg = optarg;
        break;
      case '?':
        printf("Unknown option -%c\n", c);
        break;
      default:
        return -1;
    }
  }

  if (filepath_arg == NULL) {
    printf("Filepath is a required argument\n");
    print_usage(argv);
    return 0;
  }

  if (port == 0) {
    printf("Port not set\n");
    print_usage(argv);
    return 0;
  }

  if (newfile_arg) {
    dbfd = create_db_file(filepath_arg);
    if (dbfd == STATUS_ERROR) {
      printf("Unable to create database file\n");
      return -1;
    }

    if (create_db_header(dbfd, &header) == STATUS_ERROR) {
      printf("Failed to create database header\n");
      return -1;
    }
  } else {
    dbfd = open_db_file(filepath_arg);
    if (dbfd == STATUS_ERROR) {
      printf("Unable to open database file\n");
      return -1;
    }

    if (validate_db_header(dbfd, &header) == STATUS_ERROR) {
      printf("Failed to validate database header\n");
      return -1;
    }

  }

  if (read_employees(dbfd, header, &employees) != STATUS_SUCCESS) {
    printf("Failed to read employees\n");
  }

  if (add_string_arg) {
    header->count++;
    if (realloc(employees, header->count*sizeof(struct employee_t)) == NULL) {
      printf("Realloc failed\n");
      return -1;
    }
    add_employee(header, employees, add_string_arg);
  }

  if (list_arg) {
    list_employees(header, employees);
  }

  poll_loop(port, header, employees, dbfd);

  output_file(dbfd, header, employees);

  return 0;
}
