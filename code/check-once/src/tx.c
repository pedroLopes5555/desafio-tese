#include "../include/clock.h"
#include "../include/stm_tx.h"
#include <stdint.h>
#include <sys/types.h>

/*orec is 64 bits, last bit is the lock bit*/
/*
63                0
+-----------------+
| L |  timestamp  |
+-----------------+
*/

typedef _Atomic(uint64_t) orec_t;

static int is_orec_locked(uint64_t orec) {
  // just check the MSB
  // https://stackoverflow.com/questions/58685415/how-to-test-the-most-significant-bit-of-signed-or-unsigned-integer
  const uint64_t MSB = (uint64_t)1 << (sizeof(uint64_t) * 8 - 1);
  return (orec & MSB) != 0;
}

static _Thread_local stm_tx_t tx; // one tx per thread
// begin
void tx_begin() {
  // get start timestamp
  uint64_t start_timestamp = read_timestamp();

  // clear tx
  tx.start_timestamp = 0;
  tx.end_timestamp = 0;
  tx.r_count = 0;
  tx.w_count = 0;
}

uint64_t tx_read(void *addr) {
  // check if it is a write address

  uint64_t v = *(uint64_t *)addr;
}
