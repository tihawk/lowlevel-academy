#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

#define PROTO_VERSION 100
#define BUFF_SIZE 4096

#define STATUS_ERROR    -1
#define STATUS_SUCCESS  0

typedef enum {
  MSG_ERROR,
  MSG_HELLO_REQ,  
  MSG_HELLO_RESP,  
  MSG_EMPLOYEE_LIST_REQ,
  MSG_EMPLOYEE_LIST_RESP,
  MSG_EMPLOYEE_ADD_REQ,
  MSG_EMPLOYEE_ADD_RESP,
  MSG_EMPLOYEE_DEL_REQ,
  MSG_EMPLOYEE_DEL_RESP,
} dbproto_type_e;

typedef struct {
 dbproto_type_e type;
 uint16_t length;
} dbproto_header_t;

typedef struct {
 uint16_t proto; 
} dbproto_hello_req;

typedef struct {
 uint16_t proto; 
} dbproto_hello_resp;

typedef struct {
	uint8_t data[1024];
} dbproto_employee_add_req;

#endif
