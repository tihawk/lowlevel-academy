#include "../../include/server_poll.h"
#include "../../include/common.h"
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>

void init_clients(clientstate_t* states) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    states[i].fd = -1; // -1 indicates a free slot
    states[i].state = STATE_NEW;
    memset(&states[i].buffer, '\0', BUFF_SIZE);
  }
}

int find_free_slot(clientstate_t* states) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (states[i].fd == -1) {
      return i;
    }
  }
  return -1; // No free slot found
}

int find_slot_by_fd(clientstate_t* states, int fd) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (states[i].fd == fd) {
      return i;
    }
  }
  return -1; // Not found
}

void fsm_reply_hello(clientstate_t *state, dbproto_header_t *proto_header) {
  proto_header->type = MSG_HELLO_RESP;
  proto_header->length = 1;

  dbproto_hello_resp *hello_res = (dbproto_hello_resp*) &proto_header[1];
  hello_res->proto = PROTO_VERSION;

  proto_header->type = htonl(proto_header->type);
  proto_header->length = htons(proto_header->length);
  hello_res->proto = htons(hello_res->proto);

  write(state->fd, proto_header, sizeof(dbproto_header_t) + sizeof(dbproto_hello_resp));
}

void fsm_reply_hello_error(clientstate_t *state, dbproto_header_t *proto_header) {
  proto_header->type = MSG_ERROR;
  proto_header->length = 0;

  proto_header->type = htonl(proto_header->type);
  proto_header->length = htons(proto_header->length);

  write(state->fd, proto_header, sizeof(dbproto_header_t));
}

void handle_client_fsm(struct db_header_t *db_header, struct employee_t **employees, clientstate_t *state, int dbfd) {
  dbproto_header_t *proto_header = (dbproto_header_t*) state->buffer;

  proto_header->type = ntohl(proto_header->type);
  proto_header->length = ntohs(proto_header->length);

  if (state->state == STATE_HELLO) {
    if (proto_header->type != MSG_HELLO_REQ || proto_header->length != 1) {
      printf("Didn't get MSG_HELLO in HELLO state...\n");
      // TODO: send error message
    }

    dbproto_hello_req* hello_req = (dbproto_hello_req*) &proto_header[1]; // Stupid pointer magic again
    hello_req->proto = ntohs(hello_req->proto);
    if (hello_req->proto != PROTO_VERSION) {
      printf("Protocol mismatch...\n");
      fsm_reply_hello_error(state, proto_header);
    }

    fsm_reply_hello(state, proto_header);
    state->state = STATE_MSG;
    printf("Client upgraded to STATE_MSG\n");
  }
  if (state->state == STATE_MSG) {

  }
}
