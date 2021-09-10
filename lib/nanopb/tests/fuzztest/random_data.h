/* This module handles generating & modifying messages randomly for the fuzz test. */

#ifndef RANDOM_DATA_H
#define RANDOM_DATA_H

#include <pb.h>

void random_set_seed(uint32_t seed);
uint32_t random_get_seed();

/* Random 32-bit integer */
uint32_t rand_word();

/* Get a random integer in range, with approximately flat distribution. */
int rand_int(int min, int max);

/* Random boolean, equal probability */
bool rand_bool();

/* Get a random byte, with skewed distribution.
 * Important corner cases like 0xFF, 0x00 and 0xFE occur more
 * often than other values. */
uint8_t rand_byte();

/* Get a random length, with skewed distribution.
 * Favors the shorter lengths, but always atleast 1. */
size_t rand_len(size_t max);

/* Fills a buffer with random bytes with skewed distribution. */
void rand_fill(uint8_t *buf, size_t count);

/* Fill with random protobuf-like data */
size_t rand_fill_protobuf(uint8_t *buf, size_t min_bytes, size_t max_bytes, int min_tag);

/* Given a buffer of data, mess it up a bit by copying / swapping bytes */
void rand_mess(uint8_t *buf, size_t count);

/* Append or prepend protobuf noise, with tag values > 1000 */
void rand_protobuf_noise(uint8_t *buffer, size_t bufsize, size_t *msglen);




#endif
