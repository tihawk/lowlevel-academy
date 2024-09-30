#include <stdio.h>
#include <pthread.h>

#define THREAD_COUNT 10

int counter = 0;

typedef struct {
  int arg1;
  short arg2;
} thread_arg_t;

void *thread_target(void* vargs) {
  // Critical section - we need to make sure that it's only used by one thread
  // mutex (mutual exclusion), semaphores
  for (int i = 0; i < 1000000; i++) {
    counter++;
  }
  printf("Counter is %d\n", counter);
}

int main(int argc, char *argv[]) {
  pthread_t threads[THREAD_COUNT];

  thread_arg_t myargs;

  for (int i = 0; i < THREAD_COUNT; i++) {
    // Race condition yay
    myargs.arg1 = i;
    if (pthread_create(&threads[i], NULL, thread_target, (void*)&myargs)) {
      return -1;
    }
  }

  for (int j = 0; j < THREAD_COUNT; j++) {
    pthread_join(threads[j], NULL);
  }

  return 0;
}
