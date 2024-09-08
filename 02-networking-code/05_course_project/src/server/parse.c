#include <netinet/in.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <arpa/inet.h>

#include "../../include/parse.h"
#include "../../include/common.h"

int create_db_header(int fd, struct db_header_t **headerOut) {
  struct db_header_t *header = calloc(1, sizeof(struct db_header_t));
  if (header == NULL) {
    printf("Malloc failed to create db header\n");
    return STATUS_ERROR;
  }

  header->version = 0x01;
  header->count = 0;
  header->magic = HEADER_MAGIC;
  header->filesize = sizeof(struct db_header_t);

  *headerOut = header;
  return STATUS_SUCCESS;
}

int validate_db_header(int fd, struct db_header_t **headerOut) {
  if (fd < 0) {
    printf("Got a bad FD from the user\n");
    return STATUS_ERROR;
  }

  struct db_header_t *header = calloc(1, sizeof(struct db_header_t));
  if (header == NULL) {
    printf("Malloc failed to create db header\n");
    return STATUS_ERROR;
  }

  if (read(fd, header, sizeof(struct db_header_t)) != sizeof(struct db_header_t)) {
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

int read_employees(int fd, struct db_header_t *header, struct employee_t **employeesOut) {
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

int add_employee(struct db_header_t *header, struct employee_t **employees_ptr, char *addString) {
  printf("Current employee entries: %d\n", header->count);

  char *name = strtok(addString, ",");
  if (name == NULL) {
    return STATUS_ERROR;
  }
  char *address = strtok(NULL, ",");
  if (address == NULL) {
    return STATUS_ERROR;
  }
  char *hours = strtok(NULL, ",");
  if (hours == NULL || atoi(hours) == 0) {
    return STATUS_ERROR;
  }

  header->count++;
  *employees_ptr = realloc(*employees_ptr, header->count*sizeof(struct employee_t));
  if (employees_ptr == NULL) {
    printf("Realloc failed\n");
    return STATUS_ERROR;
  }

  printf("name: %s; address: %s; hours: %s;\n", name, address, hours);

  struct employee_t *employees = *employees_ptr;

  strncpy(employees[header->count-1].name, name, sizeof(employees[header->count-1].name));
  strncpy(employees[header->count-1].address, address, sizeof(employees[header->count-1].address));
  employees[header->count-1].hours = atoi(hours);

  return STATUS_SUCCESS; 
}

void list_employees(struct db_header_t *header, struct employee_t *employees) {
  for (int i = 0; i < header->count; i++) {
    printf("Employee %d\n", i);
    printf("\tName: %s\n", employees[i].name);
    printf("\tAddress: %s\n", employees[i].address);
    printf("\tHours: %d\n", employees[i].hours);
  }
}

void output_file(int fd, struct db_header_t *header, struct employee_t *employees) {
  if (fd < 0) {
    printf("Got a bad FD from the user\n");
    return;
  }

  int host_count = header->count;
  int new_filesize = sizeof(struct db_header_t) + sizeof(struct employee_t)*host_count;

  header->version = htons(header->version);
  header->count = htons(header->count);
  header->magic = htonl(header->magic);
  header->filesize = htonl(new_filesize);

  lseek(fd, 0, SEEK_SET);

  write(fd, header, sizeof(struct db_header_t));

  for (int i = 0; i < host_count; i++) {
    employees[i].hours = htonl(employees[i].hours);  
    write(fd, &employees[i], sizeof(struct employee_t));
    employees[i].hours = ntohl(employees[i].hours);  
  }

  header->version = ntohs(header->version);
  header->count = ntohs(header->count);
  header->magic = ntohl(header->magic);
  header->filesize = ntohl(new_filesize);

  return;
}
