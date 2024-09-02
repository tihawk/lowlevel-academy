#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define MAX_EMPLOYEES 1000

struct employee_t {
  int id;
  int income;
  bool staff;
};

void initialize_employee(struct employee_t *e) {
  e->id = 0;
  e->income = 100;
  e->staff = false;
}

int main() {

  // avoid this: struct employee_t employees[MAX_EMPLOYEES];
 
  // this value "came" from the "database header"
  int n = 4;

  // 1. allocate
  struct employee_t *employees = malloc(sizeof(struct employee_t)*n);
  // 2. check if malloc failed
  if (employees == NULL) {
    printf("The allocator failed\n");
    return -1;
  }

  initialize_employee(&employees[0]);

  printf("income: %d\n", employees[0].income);

  // 3. free memory
  free(employees);
  // 4. set employees to NULL to avoid bugs and potential exploitable vulnerabilities
  employees = NULL;

  return 0;
}
