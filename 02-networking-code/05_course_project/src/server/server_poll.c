#include "../../include/server_poll.h"
#include "../../include/common.h"
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
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

void fsm_reply_add(clientstate_t *state, dbproto_header_t *proto_header) {
  proto_header->type = MSG_EMPLOYEE_ADD_RESP;
  proto_header->length = 0;

  proto_header->type = htonl(proto_header->type);
  proto_header->length = htons(proto_header->length);

  write(state->fd, proto_header, sizeof(dbproto_header_t));
}

void fsm_reply_add_error(clientstate_t *state, dbproto_header_t *proto_header) {
  proto_header->type = MSG_ERROR;
  proto_header->length = 0;

  proto_header->type = htonl(proto_header->type);
  proto_header->length = htons(proto_header->length);

  write(state->fd, proto_header, sizeof(dbproto_header_t));
}

void fsm_reply_list(struct db_header_t *db_header, struct employee_t **employees_ptr,
    clientstate_t *state) {
  dbproto_header_t *proto_header = (dbproto_header_t *) &state->buffer;
  proto_header->type = htonl(MSG_EMPLOYEE_LIST_RESP);
  proto_header->length = htons(db_header->count);

  write(state->fd, proto_header, sizeof(dbproto_header_t));

  // We will overwrite the same pointed object in the loop, immediately sending
  // it to the client, and so we don't need
  // to allocate memory or move the pointer in the buffer for each of the employees
  dbproto_employee_list_resp *employee_resp =
    (dbproto_employee_list_resp *) &proto_header[1];

  struct employee_t *employees = *employees_ptr;

  for (int i = 0; i < db_header->count; i++) {
    strncpy(employee_resp->name, employees[i].name, sizeof(employee_resp->name));
    strncpy(employee_resp->address, employees[i].address, sizeof(employee_resp->address));
    employee_resp->hours = htonl(employees[i].hours);
    write(state->fd, employee_resp, sizeof(dbproto_employee_list_resp));
  }
}

void handle_client_fsm(struct db_header_t *db_header,
    struct employee_t **employees, clientstate_t *state, int dbfd) {
  dbproto_header_t *proto_header = (dbproto_header_t*) state->buffer;

  proto_header->type = ntohl(proto_header->type);
  proto_header->length = ntohs(proto_header->length);

  if (state->state == STATE_HELLO) {
    if (proto_header->type != MSG_HELLO_REQ || proto_header->length != 1) {
      printf("Didn't get MSG_HELLO in HELLO state...\n");
      fsm_reply_hello_error(state, proto_header);
      return;
    }

    dbproto_hello_req* hello_req = (dbproto_hello_req*) &proto_header[1]; // Stupid pointer magic again
    hello_req->proto = ntohs(hello_req->proto);
    if (hello_req->proto != PROTO_VERSION) {
      printf("Protocol mismatch...\n");
      fsm_reply_hello_error(state, proto_header);
      return;
    }

    fsm_reply_hello(state, proto_header);
    state->state = STATE_MSG;
    printf("Client upgraded to STATE_MSG\n");
  } else if (state->state == STATE_MSG) {
    if (proto_header->type == MSG_EMPLOYEE_ADD_REQ && proto_header->length == 1) {
      printf("Processing MSG_EMPLOYEE_ADD_REQ\n");

      dbproto_employee_add_req *employee_add_req =
        (dbproto_employee_add_req *) &proto_header[1];

      printf("Adding employee %s\n", employee_add_req->data);
      if (add_employee(db_header, employees, employee_add_req->data) != STATUS_SUCCESS) {
        printf("Failed to add employee\n");
        fsm_reply_add_error(state, proto_header);
        return;
      } else {
        fsm_reply_add(state, proto_header);
        output_file(dbfd, db_header, *employees);
      }
    } else if (proto_header->type == MSG_EMPLOYEE_LIST_REQ) {
      printf("Sending a list of employees\n");
      fsm_reply_list(db_header, employees, state);
    }
  }
}
