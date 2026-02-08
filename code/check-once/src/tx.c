#include "../include/clock.h"
#include "../include/stm_tx.h"
#include <stdint.h>
#include <sys/types.h>

static _Thread_local stm_tx_t tx; // one tx per thread
// begin
void tx_begin() {
  // get start timestamp
  // clear tx
  tx.start_timestamp = read_timestamp();
  ;
  tx.end_timestamp = 0;
  tx.r_count = 0;
  tx.w_count = 0;
}

uint64_t tx_read(void *addr) {

  // check if it is a write address
  for (int i = 0; i < tx.w_count; i++) {
    if (tx.writes[i].addr == addr)
      return tx.writes[i].value;
  }

  // get the value
  uint64_t v = *(uint64_t *)addr;
  // get the orecs
}
