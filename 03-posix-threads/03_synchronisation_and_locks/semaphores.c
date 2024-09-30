#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define THREAD_COUNT 10

// Mutex -> mutual exclusion of access
// Semaphore -> multiple access through signaling

sem_t sem;

void *signal_event() {
  printf("Doing some work...\n");
  sleep(2);
  printf("Signaling event completion!\n");
  sem_post(&sem);

  return;
}

void *wait_for_event() {
  printf("Waiting for event...\n");
  sem_wait(&sem);
  printf("Event has been triggered!\n");

  return;
}

int main(int argc, char *argv[]) {
  pthread_t t1, t2;

  sem_init(&sem, 0, 0);

  pthread_create(&t1, NULL, wait_for_event, NULL);
  pthread_create(&t2, NULL, signal_event, NULL);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  sem_destroy(&sem);

  return 0;
}
