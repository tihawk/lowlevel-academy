#include <stdio.h>
#include <pthread.h>

__thread int x_thd;

void *wait_for_event() {
  x_thd++;
  printf("%d\n", x_thd);
  return NULL;
}

int main (int argc, char *argv[]) {
  pthread_t threads[10];

  for (int i = 0; i < 10; i++) {
    pthread_create(&threads[i], NULL, wait_for_event, NULL);
  }

  for (int i = 0; i < 10; i++) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}
