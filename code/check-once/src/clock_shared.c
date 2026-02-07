#include "../include/clock.h"
#include <stdatomic.h>

static atomic_uint_fast64_t ts = 0;

uint64_t read_timestamp(void) { return atomic_load(&ts); }

uint64_t getNext_timestamp(void) { return atomic_fetch_add(&ts, 1) + 1; }
