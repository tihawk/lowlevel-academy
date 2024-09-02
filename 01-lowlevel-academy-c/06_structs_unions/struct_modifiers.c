#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_EMPLOYEES 1000

// This will assure, that no additional padding is introduced
// in the process of building the code for different architectures.
// This might be necessary in the case where different architectures
// might have to communicate with each-other using this struct,
// so we have to make sure they look identical on different platforms.
struct __attribute__((__packed__)) employee_t {
  int id;
  char firstName[64];
  char lastName[64];
  float income;
  bool isManager;
};

int main() {

  struct employee_t Fred;
  Fred.income = 100.00;
  Fred.isManager = true;

  struct employee_t employees[MAX_EMPLOYEES];

  int i = 0;
  for (i = 0; i < MAX_EMPLOYEES; i++) {
    employees[i].income = 0;
    employees[i].isManager = false;
    employees[i].id = i;
    strcpy(employees[i].firstName, "Steve");
    printf("%s: isManager: %s, income: %f\n", 
        employees[i].firstName, 
        employees[i].isManager ? "true" : "false", 
        employees[i].income);
  }


  return 0;
}
