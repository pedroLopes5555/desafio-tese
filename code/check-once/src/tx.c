#include "../include/clock.h"
#include "../include/orec.h"
#include "../include/stm_tx.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
void ix_begin(void);
int tx_read_int(void *addr);
void tx_write(void *addr, int v);
void aquire_locks(void);
void abort(void);
void tx_commit(void);
void write_back();
void validate();
void release_locks();

static _Thread_local stm_tx_t tx; // one tx per thread
// begin
void tx_begin() {
  // get start timestamp
  // clear tx
  tx.start_timestamp = read_timestamp();
  tx.end_timestamp = 0;
  tx.r_count = 0;
  tx.w_count = 0;
  tx.lock_count = 0;
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
  return 0;
  // Abort();
}

void tx_write(void *addr, int v) {
  // TODO -> check if the addres is valid
  stm_tx_write_t write;
  printf("tx_write: w_count=%d addr=%p value=%d\n", tx.w_count, addr, v);
  write.addr = addr;
  write.value = (uint64_t)v;

  tx.writes[tx.w_count] = write;

  tx.w_count++;
}

void aquire_locks() {
  // try to aquire_locks of the writes

  for (int i = 0; i < tx.w_count; i++) {

    printf("try to aquire lock of write -> %d", tx.writes[i].value);
    if (try_aquire_lock(tx.writes[i].addr) == 0) {
      tx.locked_addrs[tx.lock_count] = tx.writes[i].addr;
      tx.lock_count++;
    } else {

      return; // TODO -> latter abort
    }
  }
}

void abort() {
  for (int i = 0; i < tx.lock_count; i++) {
    release_lock_no_end(tx.locked_addrs[i]);
  }
}

void tx_commit() {
  if (tx.w_count == 0)
    return;

  aquire_locks();
  validate();
  write_back();
  tx.end_timestamp = read_timestamp();
  release_locks();
}

void write_back() {
  for (int i = 0; i < tx.w_count; i++) {
    int *addr = (int *)tx.writes[i].addr;
    *addr = (int)tx.writes[i].value;
    printf("write_back: w_count=%d addr=%p value=%d\n", tx.w_count,
           tx.writes[i].addr, (int)tx.writes[i].value);
  }
}

void validate() {

  for (int i = 0; i < tx.w_count; i++) {
    uint64_t ts = get_addrs_timestamp(tx.writes[i].addr);

    if (ts >= tx.start_timestamp) {
      printf("Abort");
      break; // TODO -> abort
    }
  }
}

void release_locks() {
  for (int i = 0; i < tx.w_count; i++) {
    // sleep(15);
    release_lock(tx.end_timestamp, tx.writes[i].addr);
  }
}
