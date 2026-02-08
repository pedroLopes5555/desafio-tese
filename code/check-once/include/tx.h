#include <stdint.h>
void tx_begin();

int tx_read_int(void *addr);

void tx_write(void *addr, int v);

void tx_commit(void);
