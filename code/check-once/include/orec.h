
#include <stddef.h>
#include <stdint.h>

typedef _Atomic(uint64_t) orec_t;

extern orec_t *orecs;
extern size_t num_orecs;

void orecs_init(int number_of_orecs, int data_type_size,
                void *first_data_element);

int is_orec_locked(uint64_t orec);

static uint64_t get_orec_timestamp(uint64_t orec);

uint64_t get_addrs_timestamp(void *addr);

int is_addrs_orec_locked(void *addr);

int try_aquire_lock(void *addr);

void release_lock(uint64_t end, void *addr);

void release_lock_no_end(void *addr);

void orecs_destroy(void);
