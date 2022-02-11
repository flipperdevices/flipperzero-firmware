#pragma once

#include <inttypes.h>

#define HASH_LENGTH 20
#define BLOCK_LENGTH 64

void init(void);
void initHmac(const uint8_t* secret, uint8_t secretLength);
uint8_t* result(void);
uint8_t* resultHmac(void);
void write(uint8_t);
void writeArray(uint8_t* buffer, uint8_t size);
