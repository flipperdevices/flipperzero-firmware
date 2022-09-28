#ifndef _sha1_h
#define _sha1_h

#include <inttypes.h>

#define HASH_LENGTH 20
#define BLOCK_LENGTH 64

void sha1_init(void);
void sha1_init_hmac(const uint8_t* secret, uint8_t secretLength);
uint8_t* sha1_result(void);
uint8_t* sha1_result_hmac(void);
void sha1_write(uint8_t);
void sha1_write_array(uint8_t *buffer, uint8_t size);
#endif
