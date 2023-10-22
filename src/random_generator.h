#ifndef __RANDOM_GENERATOR_H__
#define __RANDOM_GENERATOR_H__

#include <stdint.h>
#include <furi_hal.h> // For the random generator

/* Return a random number uniformly distributed in
 * the interval [min, max) */
uint32_t random_uniform(uint32_t, uint32_t);

/* Return a random value with a given probability
 * of being true. The argument must be in the range
 * [0, 100] */
bool toss_a_coin(uint32_t);
#endif
