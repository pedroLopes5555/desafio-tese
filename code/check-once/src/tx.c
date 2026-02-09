#include "../include/clock.h"
#include "../include/orec.h"
#include "../include/stm_tx.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void tx_begin(void);
int tx_read_int(void *addr);
void tx_write(void *addr, int v);
int aquire_locks(void);
void tx_abort(void);
void tx_commit(void);
void write_back();
void validate();
void release_locks();

static _Thread_local stm_tx_t tx; // one tx per thread
// begin
void tx_begin() {
  if (setjmp(tx.env) != 0) { // in theroy, longjmp will go to here
    printf("tx restart\n");
  }

  tx.start_timestamp = read_timestamp();
  tx.end_timestamp = 0;
  tx.r_count = 0;
  tx.w_count = 0;
  tx.lock_count = 0;
}

int tx_read_int(void *addr) {
  for (int i = 0; i < tx.w_count; i++) {
    if (tx.writes[i].addr == addr)
      return (int)tx.writes[i].value;
  }

  int v = *(int *)addr;

  uint64_t ts = get_addrs_timestamp(addr);
  int locked = is_addrs_orec_locked(addr);

  if (locked || ts > tx.start_timestamp) {
    tx_abort();
  }

  stm_tx_read_t read;
  read.addr = addr;
  tx.reads[tx.r_count] = read;
  tx.r_count++;

  return v;
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

int aquire_locks(void) {
  for (int i = 0; i < tx.w_count; i++) {
    void *addr = tx.writes[i].addr;

    if (try_aquire_lock(addr) != 1) {
      tx_abort();
    }

    tx.locked_addrs[tx.lock_count++] = addr;
  }
  return 1;
}

void tx_abort() {
  for (int i = 0; i < tx.lock_count; i++) {
    release_lock_no_end(tx.locked_addrs[i]);
  }

  longjmp(tx.env, 1); // jump back to begin
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

  for (int i = 0; i < tx.r_count; i++) {
    uint64_t ts = get_addrs_timestamp(tx.reads[i].addr);

    if (ts > tx.start_timestamp) {
      printf("Abort");
      tx_abort();
    }
  }
}

void release_locks() {
  for (int i = 0; i < tx.w_count; i++) {
    release_lock(tx.end_timestamp, tx.writes[i].addr);
  }
}
