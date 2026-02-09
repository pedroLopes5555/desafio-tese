#include "../include/clock.h"
#include "../include/orec.h"
#include "../include/tx.h"
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#define VALUE_SIZE sizeof(int);

// for this example we will assume that data is alwais an array
// for example of integers

int *data;

void ini_data(unsigned long size) {
  data = malloc(sizeof(int) * size);

  for (int i = 0; i < size; i++) {
    data[i] = i;
  }
}

static void *main_thread(void *thread_id) {
  (void)thread_id;

  tx_begin();

  tx_write(&data[1], 100);

  tx_commit();

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

  // initilization
  ini_data(data_size);
  orecs_init(data_size, sizeof(int),
             &data[0]); // 100% sure this is not the best way to do this just
                        // praying for this to work praying for this to work

  printf("CHEGOY AQUI\n");
  fflush(stdout);

  pthread_t th[num_threads];
  int i;
  for (i = 0; i < num_threads; i++) {
    if (pthread_create(&th[i], NULL, main_thread, (void *)(intptr_t)i)) {
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

  for (int i = 0; i < data_size; i++) {
    printf("for data index %d ->", i);
    printf("data value : %d | ", data[i]);
    printf("is data locked: %d\n", is_addrs_orec_locked(&data[i]));
  }

  free(data);

  return EXIT_SUCCESS;
}
