#include <stdio.h>
#include <pthread.h>

#define THREAD_COUNT 10

int counter = 0;
pthread_mutex_t counter_lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
  int arg1;
  short arg2;
} thread_arg_t;

void *thread_target(void* vargs) {
  // Critical section - we need to make sure that it's only used by one thread
  // Mutex
  // 1 thread gets to pass
  // 9 threads are in starvation
  pthread_mutex_lock(&counter_lock);
  
  for (int i = 0; i < 1000000; i++) {
    counter++;
  }

  pthread_mutex_unlock(&counter_lock);

  printf("Counter is %d\n", counter);
}

int main(int argc, char *argv[]) {
  pthread_t threads[THREAD_COUNT];

  pthread_mutex_init(&counter_lock, NULL);


  for (int i = 0; i < THREAD_COUNT; i++) {

    if (pthread_create(&threads[i], NULL, thread_target, NULL)) {
      return -1;
    }
  }

  for (int j = 0; j < THREAD_COUNT; j++) {
    pthread_join(threads[j], NULL);
  }

  return 0;
}
