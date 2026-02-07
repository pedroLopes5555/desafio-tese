#ifndef CLOCK_SHARED_H
#define CLOCK_SHARED_H

#include <stdint.h>

/*
 * @breaf returns the last timestamp
 */
uint64_t read_timestamp(void);

/*
 *@breaf returns and adds to the next timestamp
 * */
uint64_t getNext_timestamp(void);

#endif
