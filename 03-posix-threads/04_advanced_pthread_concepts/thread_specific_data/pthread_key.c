#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_key_t key;

void array_desctructor(void *arr) {
  free(arr);
  printf("Array freed for a thread\n");
}

void *thread_func(void *arg) {
  int *my_array = (int *) malloc(10 * sizeof(int));

  pthread_setspecific(key, my_array);

  for (int i = 0; i < 10; i++) {
    my_array[i] = i;
  }

  for (int i = 0; i < 10; i++) {
    printf("%d ", my_array[i]);
  }
  printf("\n");

  return NULL;
}

int main(int argc, char *argv[]) {
  pthread_t thread1, thread2;

  pthread_key_create(&key, array_desctructor);

  pthread_create(&thread1, NULL, thread_func, NULL);
  pthread_create(&thread2, NULL, thread_func, NULL);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  pthread_key_delete(key);

  return 0;
}
