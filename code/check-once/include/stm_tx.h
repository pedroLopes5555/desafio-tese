#define STM_TX_H

#include <stdint.h>

#define MAX_WRITES 64
#define MAX_READS 64

typedef struct {
  void *addr;     /* address to write */
  uint64_t value; /* value to write */
  uint64_t *orec; /* pointer to orec / version */
} stm_tx_write_t;

typedef struct {
  void *addr;     /* address read */
  uint64_t *orec; /* pointer to orec / version */
} stm_tx_read_t;

/*
 * Transaction metadata (per-thread)
 */
typedef struct {
  uint64_t start_timestamp;
  uint64_t end_timestamp;

  stm_tx_write_t writes[MAX_WRITES];
  int w_count;

  stm_tx_read_t reads[MAX_READS];
  int r_count;

  // locks

} stm_tx_t;
