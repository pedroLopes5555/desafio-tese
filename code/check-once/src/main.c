#include "../include/clock.h"
#include "../include/orec.h"
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

// to test we will make the threads make changes to an array
static void *main_thread(void *thread_id) {
  long tid = (long)thread_id;
  printf("%ld\n", tid);
  for (int i = 0; i < 10; i++) {
    getNext_timestamp();
  }

  return NULL;
}

// main
int main(int argc, char *argv[]) {

  if (argc != 3) {
    printf("example of use: \n");
    printf("./test data_size num_threads\n");
    return 0;
  }

  int data_size = atoi(argv[1]);
  int num_threads = atoi(argv[2]);

  orecs_init(data_size);

  pthread_t th[num_threads];
  int i;
  for (i = 0; i < num_threads; i++) {
    if (pthread_create(&th[i], NULL, main_thread, (void *)i) != 0) {
      perror("pthread_create");
      return 1;
    }
  }

  for (int i = 0; i < num_threads; i++) {
    pthread_join(th[i], NULL);
  }

  uint64_t final = read_timestamp();
  printf("final value = %llu (expected %llu)\n", (unsigned long long) final,
         (unsigned long long)num_threads * (unsigned long long)num_threads);
  return EXIT_SUCCESS;
}
