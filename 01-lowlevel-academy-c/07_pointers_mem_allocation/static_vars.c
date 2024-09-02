#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define MAX_EMPLOYEES 1000

struct employee_t {
  int id;
  int income;
  bool staff;
};

int initialize_employee(struct employee_t *e) {
  // static vars only get set once, the first time the function is called.
  static int numEmployees = 0;
  // only we can change the static variable
  numEmployees++;

  e->id = numEmployees;
  e->income = 100;
  e->staff = false;

  return numEmployees;
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

  for (int i = 0; i < n; i++) {
    int id = initialize_employee(&employees[i]);
    printf("New employee with ID %d\n", id);
  }

  // 3. free memory
  free(employees);
  // 4. set employees to NULL to avoid bugs and potential exploitable vulnerabilities
  employees = NULL;

  return 0;
}
