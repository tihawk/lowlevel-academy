#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define _GNU_SOURCE
#include <sched.h>

void* thread_function(void* arg) {
  pthread_t tid = pthread_self();
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);

  if (pthread_getaffinity_np(tid, sizeof(cpu_set_t), &cpuset) == 0) {
    printf("Thread %lu runnin on CPUs: ", (unsigned long)tid);
    for (int i = 0; i < CPU_SETSIZE; i++) {
      if (CPU_ISSET(i, &cpuset)) {
        printf("%d", i);
      }
    }
    printf("\n");
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  pthread_t thread;
  pthread_attr_t attr;
  cpu_set_t cpuset;
  struct sched_param param;

  pthread_attr_init(&attr);

  size_t stack_size = 1024 * 1024;
  pthread_attr_setstacksize(&attr, stack_size);

  pthread_attr_setschedpolicy(&attr, SCHED_RR); // "Round robin" scheduling

  param.sched_priority = sched_get_priority_max(SCHED_RR);
  pthread_attr_setschedparam(&attr, &param);
  pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

  CPU_ZERO(&cpuset);
  CPU_SET(0, &cpuset);
  pthread_attr_setaffinity_np(&attr, CPU_SETSIZE, &cpuset);

  if (pthread_create(&thread, &attr, thread_function, NULL) != 0) {
    perror("Failed to create thread");
    return 1;
  }

  pthread_join(thread, NULL);
  pthread_attr_destroy(&attr);

  return 0;
}
