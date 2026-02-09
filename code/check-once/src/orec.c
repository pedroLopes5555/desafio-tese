/*
 *THERE IS A LOT OF BAD CODE AND REDUNDANCY ON THIS FILE * * */

#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <inttypes.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
/*orec is 64 bits, last bit is the lock bit*/
/*
63                0
+-----------------+
| L |  timestamp  |
+-----------------+
*/

#define LOCK_BIT (1ULL << 63)
#define TS_MASK (LOCK_BIT - 1) // low 63 bits

/*
 * shifting to << we can have th MSB
10000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
^
\
63
*/
typedef _Atomic(uint64_t) orec_t;

orec_t *orecs;
static size_t num_orecs;
static int dt_size;
static void *ft_data_element;
/*
 *  \
 *  V
 * this is kinda dangerous, if
 * we use it with out the initioation method being called
 *

 * in this implementation the number of orecs should be the same number of a
 * data array the map betwin orecs and data is trivial
 *
 *data -> [] [] [] [] []
 *        \  \  \  \   \
 *orec -> [] [] [] [] []
 *
 *
 *the only proble is that i cant abstracr the orecs initioation
 *on the main funtion i should do orecs_init
 * */
int is_orec_locked(uint64_t orec) { return (orec & LOCK_BIT) != 0; }
void orecs_destroy(void) {
  free(orecs);
  orecs = NULL;
  num_orecs = 0;
}
void orecs_init(int number_of_orecs, int data_type_size,
                void *first_data_element) {
  num_orecs = (size_t)number_of_orecs;

  orecs = malloc(num_orecs * sizeof(orec_t));

  for (size_t i = 0; i < num_orecs; i++) {
    atomic_store(&orecs[i], 0); // unlocked, timestamp = 0
  }

  // TEST ONLY: force orec[1] to start locked (MSB = 1, timestamp = 0)
  // atomic_store_explicit(&orecs[1], (1ULL << 63), memory_order_relaxed);

  dt_size = (size_t)data_type_size;
  ft_data_element = first_data_element;

  /*printf("orecs_init: locked orecs:\n");
  forr (size_t i = 0; i < num_orecs; i++) {
    uint64_t o = atomic_load_explicit(&orecs[i], memory_order_relaxed);

    if (is_orec_locked(o)) {
      uint64_t ts = o & ((1ULL << 63) - 1);
      printf("  orec[%zu] LOCKED (ts=%llu)\n", i, (unsigned long long)ts);
    } else {
      printf("  orec[%zu] unlocked (ts=%llu)\n", i,
             (unsigned long long)(o & ((1ULL << 63) - 1)));
    }
  }*/
}

static uint64_t get_orec_timestamp(uint64_t orec) {
  // make the frst bit 0 and we get the number
  return orec & ((1ULL << 63) - 1);
}

uint64_t get_orec_by_addrs(void *addr) {
  uintptr_t base = (uintptr_t)ft_data_element;
  uintptr_t a = (uintptr_t)addr;
  uintptr_t end = base + (uintptr_t)(dt_size * num_orecs);

  // check for bounds...
  if (a < base || a >= end)
    return 0; // TODO-> latter implement error message

  uintptr_t diff = a - base;

  size_t idx = (size_t)(diff / dt_size);

  uint64_t o = atomic_load(&orecs[idx]);
  return o;
}

orec_t *get_orec_ptr_by_addrs(void *addr) {
  uintptr_t base = (uintptr_t)ft_data_element;
  uintptr_t a = (uintptr_t)addr;
  uintptr_t end = base + (uintptr_t)(dt_size * num_orecs);

  // bounds check
  if (a < base || a >= end)
    return NULL;

  uintptr_t diff = a - base;
  size_t idx = (size_t)(diff / dt_size);

  return &orecs[idx]; // ✅ pointer to the real atomic orec
}
int is_addrs_orec_locked(void *addr) {

  uint64_t orec = get_orec_by_addrs(addr);
  return is_orec_locked(orec);
}

uint64_t get_addrs_timestamp(void *addr) {
  uintptr_t base = (uintptr_t)ft_data_element;
  uintptr_t a = (uintptr_t)addr;
  uintptr_t end = base + (uintptr_t)(dt_size * num_orecs);

  // check for bounds...

  if (a < base || a >= end)
    return 0; // TODO-> latter implement error message

  uintptr_t diff = a - base;

  size_t idx = (size_t)(diff / dt_size);

  uint64_t o = atomic_load(&orecs[idx]);
  return get_orec_timestamp(o);
}

int try_aquire_lock(void *addr) {
  orec_t *orecp = get_orec_ptr_by_addrs(addr);
  if (!orecp)
    return 0;
  uint64_t loaded = atomic_load_explicit(orecp, memory_order_relaxed);

  if (is_orec_locked(loaded) == 1)
    return 0;

  uint64_t expected = loaded;
  uint64_t newv = loaded | (1ULL << 63);

  if (atomic_compare_exchange_strong_explicit(
          orecp, &expected, newv, memory_order_acquire, memory_order_relaxed)) {
    return 1;
  }

  return 0;
}

void release_lock(uint64_t end, void *addr) {
  orec_t *orecp = get_orec_ptr_by_addrs(addr);

  printf("\n\nrelease lock: addr=%p value=%llu\n", addr,
         (unsigned long long)end);

  uint64_t newvalue = (end & TS_MASK);
  atomic_store_explicit(orecp, newvalue, memory_order_release);
}

void release_lock_no_end(void *addr) {
  orec_t *orecp = get_orec_ptr_by_addrs(addr);

  uint64_t current = atomic_load_explicit(orecp, memory_order_relaxed);

  uint64_t newvalue = current & TS_MASK;

  atomic_store_explicit(orecp, newvalue, memory_order_release);
}
