
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

typedef _Atomic(uint64_t) orec_t;

orec_t *orecs;
size_t num_orecs;

/*
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
void orecs_init(size_t n) {
  num_orecs = n;
  orecs = aligned_alloc(64, n * sizeof(orec_t));
  if (!orecs)
    abort();

  for (size_t i = 0; i < n; i++) {
    atomic_store(&orecs[i], 1); // unlocked, timestamp = 0
  }
}

static int is_orec_locked(uint64_t orec) {
  // just check the MSB
  // https://stackoverflow.com/questions/58685415/how-to-test-the-most-significant-bit-of-signed-or-unsigned-integer
  const uint64_t MSB = (uint64_t)1 << (sizeof(uint64_t) * 8 - 1);
  return (orec & MSB) != 0;
}

static uint64_t get_orec_timestamp(uint64_t orec) {
  // make the frst bit 0 and we get the number
  return orec & ((1ULL << 63) - 1);
}
