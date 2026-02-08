#include "../include/clock.h"
#include "../include/orec.h"
#include "../include/stm_tx.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

void tx_begin(void);
int tx_read_int(void *addr);
void tx_write(void *addr, int v);
void aquire_locks(void);
void abort(void);
void tx_commit(void);

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

void tx_write(void *addr, int v) {
  // TODO -> check if the addres is valid
  stm_tx_write_t write;

  write.addr = addr;
  write.value = v;

  tx.writes[tx.w_count] = write;

  tx.w_count++;
}

void aquire_locks() {
  // try to aquire_locks of the writes

  for (int i = 0; i < tx.w_count; i++) {

    printf("try to aquire lock of write -> %d", tx.writes[i].value);
    if (try_aquire_lock(tx.writes[i].addr) != 1) {
      return; // TODO -> latter abort
    }
  }
}

void abort() {
  /*ABORT()
13 for each addr in locks do
14 orecs[addr].releaseToPrevious()
15 restartTransaction()*/
}

void tx_commit() {
  if (tx.w_count == 0)
    return;

  aquire_locks();
  for (int i = 0; i < tx.w_count; i++) {
    int *addr = (int *)tx.writes[i].addr;
    *addr = (int)tx.writes[i].value;
  }
}
