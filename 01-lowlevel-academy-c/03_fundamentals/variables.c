#include <stdio.h>

// Global Scope
int g_NumPersons = 0;

void some_other_function() {
  int personID = 1;
}

int main() {
  // type name = initial value;
  int personID = 0;
 
  personID += 1;
  {
    int personID = 0;
    personID += 1;
  }
}
