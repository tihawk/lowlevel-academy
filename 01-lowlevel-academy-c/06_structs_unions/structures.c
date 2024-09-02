#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_EMPLOYEES 1000

struct employee_t {
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
    printf("%d: isManager: %s, income: %f\n", 
        employees[i].id, 
        employees[i].isManager ? "true" : "false", 
        employees[i].income);
  }


  return 0;
}
