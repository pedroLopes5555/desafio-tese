#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

#include "../include/clock.h"

// to test we will make the threads make changes to an array

int *data;
int *orecs;
uint64_t *tx;

void init_values(int data_size, int num_threads) {

  data = calloc(0, sizeof(int) * data_size);
  orecs = calloc(0, sizeof(uint64_t) * data_size);
  tx = calloc(0, num_threads * sizeof(uint64_t));
}

/*
 *in this transation we do not need to lock, jis to read
 * */
void read_only_transation() {
  // start transation

  // get start time
  uint64_t start = read_timestamp();
}

// thread
static void *main_thread(void *thread_id) {
  long tid = (long)thread_id;
  printf("%ld\n", tid);
  for (int i = 0; i < 10; i++) {
    getNext_timestamp();
  }

  tx[tid * sizeof(uint64_t)] = 10;
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

  // initiate values
  init_values(data_size, num_threads);

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
