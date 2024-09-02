#include <netinet/in.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <arpa/inet.h>

#include "../include/parse.h"
#include "../include/common.h"

int create_db_header(int fd, struct dbHeader_t **headerOut) {
  struct dbHeader_t *header = calloc(1, sizeof(struct dbHeader_t));
  if (header == NULL) {
    printf("Malloc failed to create db header\n");
    return STATUS_ERROR;
  }

  header->version = 0x01;
  header->count = 0;
  header->magic = HEADER_MAGIC;
  header->filesize = sizeof(struct dbHeader_t);

  *headerOut = header;
  return STATUS_SUCCESS;
}

int validate_db_header(int fd, struct dbHeader_t **headerOut) {
  if (fd < 0) {
    printf("Got a bad FD from the user\n");
    return STATUS_ERROR;
  }

  struct dbHeader_t *header = calloc(1, sizeof(struct dbHeader_t));
  if (header == NULL) {
    printf("Malloc failed to create db header\n");
    return STATUS_ERROR;
  }

  if (read(fd, header, sizeof(struct dbHeader_t)) != sizeof(struct dbHeader_t)) {
    perror("read");
    free(header);
    return STATUS_ERROR;
  }

  header->version = ntohs(header->version);
  header->count = ntohs(header->count);
  header->magic = ntohl(header->magic);
  header->filesize = ntohl(header->filesize);

  if (header->version != 1) {
    printf("Improper header version\n");
    free(header);
    return STATUS_ERROR;
  }

  if (header->magic != HEADER_MAGIC) {
    printf("Improper header magic\n");
    free(header);
    return STATUS_ERROR;
  }

  struct stat dbStat = {0};
  fstat(fd, &dbStat);
  if (header->filesize != dbStat.st_size) {
    printf("Corrupted database");
    free(header);
    return STATUS_ERROR;
  }

  *headerOut = header;

  return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbHeader_t *header, struct employee_t **employeesOut) {
  if (fd < 0) {
    printf("Got a bad FD from the user\n");
    return STATUS_ERROR;
  }

  int count = header->count;

  struct employee_t *employees = calloc(count, sizeof(struct employee_t));
  if (employees == NULL) {
    printf("Malloc failed to allocate memory for employees\n");
    return STATUS_ERROR;
  }
 
  if (read(fd, employees, count * sizeof(struct employee_t)) != count * sizeof(struct employee_t)) {
    perror("read");
    free(employees);
    return STATUS_ERROR;
  }

  int i = 0;
  for (; i < count; i++) {
    employees[i].hours = ntohl(employees[i].hours);
  }

  *employeesOut = employees;
  return STATUS_SUCCESS;
}

int add_employee(struct dbHeader_t *header, struct employee_t *employees, char *addString) {
  printf("%s\n", addString);

  char *name = strtok(addString, ",");
  char *address = strtok(NULL, ",");
  char *hours = strtok(NULL, ",");

  printf("name: %s; address: %s; hours: %s;\n", name, address, hours);

  strncpy(employees[header->count-1].name, name, sizeof(employees[header->count-1].name));
  strncpy(employees[header->count-1].address, address, sizeof(employees[header->count-1].address));
  employees[header->count-1].hours = atoi(hours);

  return STATUS_SUCCESS; 
}

void list_employees(struct dbHeader_t *header, struct employee_t *employees) {
  for (int i = 0; i < header->count; i++) {
    printf("Employee %d\n", i);
    printf("\tName: %s\n", employees[i].name);
    printf("\tAddress: %s\n", employees[i].address);
    printf("\tHours: %d\n", employees[i].hours);
  }
}

void output_file(int fd, struct dbHeader_t *header, struct employee_t *employees) {
  if (fd < 0) {
    printf("Got a bad FD from the user\n");
    return STATUS_ERROR;
  }

  int hostCount = header->count;
  int newFilesize = sizeof(struct dbHeader_t) + sizeof(struct employee_t)*hostCount;

  header->version = htons(header->version);
  header->count = htons(header->count);
  header->magic = htonl(header->magic);
  header->filesize = htonl(newFilesize);

  lseek(fd, 0, SEEK_SET);

  write(fd, header, sizeof(struct dbHeader_t));

  for (int i = 0; i < hostCount; i++) {
    employees[i].hours = htonl(employees[i].hours);  
    write(fd, &employees[i], sizeof(struct employee_t));
  }

  return;
}
