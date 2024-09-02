#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>

#include "../include/file.h"
#include "../include/common.h"
#include "../include/parse.h"

void print_usage(char *argv[]) {
  printf("Usage: %s -n -f <database file>\n", argv[0]);
  printf("\t -n - create new database file\n");
  printf("\t -f - (required) path to database file\n");
  return;
}

int main (int argc, char *argv[]) {
  
  char *addString = NULL;
  char *filepath = NULL;
  bool list = false;
  bool newfile = false;
  int c;

  // DB file descriptor
  int dbfd = -1;
  // DB header
  struct dbHeader_t *header = NULL;
  struct employee_t *employees = NULL; // Apparently it doesn't matter that it's an array of employees

  while ((c = getopt(argc, argv, "nf:a:l")) != -1) {
    switch (c) {
      case 'n':
        newfile = true;
        break;
      case 'f':
        filepath = optarg;
        break;
      case 'l':
        list = true;
        break;
      case 'a':
        addString = optarg;
        break;
      case '?':
        printf("Unknown option -%c\n", c);
        break;
      default:
        return -1;
    }
  }

  if (filepath == NULL) {
    printf("Filepath is a required argument\n");
    print_usage(argv);
    return 0;
  }


  if (newfile) {
    dbfd = create_db_file(filepath);
    if (dbfd == STATUS_ERROR) {
      printf("Unable to create database file\n");
      return -1;
    }

    if (create_db_header(dbfd, &header) == STATUS_ERROR) {
      printf("Failed to create database header\n");
      return -1;
    }
  } else {
    dbfd = open_db_file(filepath);
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

  if (addString) {
    header->count++;
    if (realloc(employees, header->count*sizeof(struct employee_t)) == NULL) {
      printf("Realloc failed\n");
      return -1;
    }
    add_employee(header, employees, addString);
  }

  if (list) {
    list_employees(header, employees);
  }

  output_file(dbfd, header, employees);

  return 0;
}
