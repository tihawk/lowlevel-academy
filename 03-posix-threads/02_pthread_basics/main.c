#include <stdio.h>
#include <pthread.h>

#define THREAD_COUNT 10

typedef struct {
  int arg1;
  short arg2;
} thread_arg_t;

void *thread_target(void* vargs) {
  thread_arg_t *args = (thread_arg_t*) vargs;
  printf("I am a thread, and %d\n", args->arg1);
  // pthread_exit() just exits the thread
  // pthread_detach() automatically releases resources for the next one
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
