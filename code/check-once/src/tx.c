#include "../include/clock.h"
#include "../include/orec.h"
#include "../include/stm_tx.h"
#include <stdint.h>
#include <sys/types.h>
static _Thread_local stm_tx_t tx; // one tx per thread
// begin
void tx_begin() {
  // get start timestamp
  // clear tx
  tx.start_timestamp = read_timestamp();
  tx.end_timestamp = 0;
  tx.r_count = 0;
  tx.w_count = 0;
}

int tx_read_int(void *addr) {

  // check if it is a write address
  for (int i = 0; i < tx.w_count; i++) {
    if (tx.writes[i].addr == addr)
      return tx.writes[i].value;
  }

  // get the value
  int v = *(uint64_t *)addr;
  // get the orecs
  uint64_t ts = get_addrs_timestamp(addr);
  int is_lockd = is_addrs_orec_locked(addr);

  // check timestamps:
  if (ts <= tx.start_timestamp && is_lockd != 1) {
    // add to reads
    // check for max reads
    tx.r_count++;
    stm_tx_read_t read;
    read.addr = addr;
    tx.reads[tx.r_count] = read;

    return v;
  }
  return 99;
  // Abort();
}
