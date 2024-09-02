#include <stdio.h>
#include <stdbool.h>

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
  int x = 3;
  int *pX = &x;

  printf("address: %p, value: %d, address of pointer itself: %p\n", pX, *pX, &pX);

  struct employee_t Ralph;

  initialize_employee(&Ralph);

  printf("income: %d\n", Ralph.income);

  return 0;
}
