#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <inttypes.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
/*orec is 64 bits, last bit is the lock bit*/
/*
63                0
+-----------------+
| L |  timestamp  |
+-----------------+
*/

#define LOCK_BIT (1ULL << 63)

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

void orecs_init(int number_of_orecs, int data_type_size,
                void *first_data_element) {
  num_orecs = (size_t)number_of_orecs;

  orecs = malloc(num_orecs * sizeof(orec_t));
  if (!orecs)
    abort();

  for (size_t i = 0; i < num_orecs; i++) {
    atomic_store(&orecs[i], 0); // unlocked, timestamp = 0
  }

  dt_size = (size_t)data_type_size;
  ft_data_element = first_data_element;
}

static uint64_t get_orec_timestamp(uint64_t orec) {
  // make the frst bit 0 and we get the number
  return orec & ((1ULL << 63) - 1);
}

int is_addrs_orec_locked(void *addr) {

  uintptr_t base = (uintptr_t)ft_data_element;
  uintptr_t a = (uintptr_t)addr;
  uintptr_t end = base + (uintptr_t)(dt_size * num_orecs);

  // check for bounds...

  if (a < base || a >= end)
    return 0; // TODO-> latter implement error message

  uintptr_t diff = a - base;

  size_t idx = (size_t)(diff / dt_size);

  uint64_t o = atomic_load(&orecs[idx]);
  return is_orec_locked(o);
}
