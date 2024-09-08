#ifndef SERVER_POLL_H
#define SERVER_POLL_H

#include <string.h>
#include <poll.h>

#include "parse.h"
#include "common.h"

#define MAX_CLIENTS 256

typedef enum {
    STATE_NEW,
    STATE_CONNECTED,
    STATE_DISCONNECTED,
    STATE_HELLO,
    STATE_MSG,
    STATE_GOODBYE,
} state_e;

// Structure to hold client state
typedef struct {
    int fd;
    state_e state;
    char buffer[BUFF_SIZE];
} clientstate_t;

void init_clients(clientstate_t* states);

int find_free_slot(clientstate_t* states);

int find_slot_by_fd(clientstate_t* states, int fd);

void handle_client_fsm(struct db_header_t *db_header, struct employee_t **employees, clientstate_t *state, int dbfd);

#endif
