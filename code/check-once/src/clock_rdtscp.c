#include "../include/clock.h"
#include <stdatomic.h>
#include <x86intrin.h>

uint64_t read_timestamp() {
  unsigned aux;

  return __rdtscp(&aux);
}

uint64_t getNext_timestamp() {
  unsigned aux;
  return __rdtscp(&aux);
}

/*#include <x86intrin.h>   // for __rdtscp

 *litle test program to see how many cpu cycles does the program make

int main(void) {

        volatile int a = 0;
unsigned int aux;

    // Read starting timestamp
    uint64_t start = __rdtscp(&aux);

    uint64_t now;

    a++;


    now = __rdtscp(&aux);
    printf("start = %llu, end = %llu, diff = %llu\n",
           (unsigned long long)start,
           (unsigned long long)now,
           (unsigned long long)(now - start));

    return 0;
}*/
