
#include <stddef.h>
#include <stdint.h>

typedef _Atomic(uint64_t) orec_t;

extern orec_t *orecs;
extern size_t num_orecs;

void orecs_init(size_t n);

static int is_orec_locked(uint64_t orec);

static uint64_t get_orec_timestamp(uint64_t orec);
