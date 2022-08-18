#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct VarintPair VarintPair;

VarintPair* varint_pair_alloc();

void varint_pair_free(VarintPair* pair);

bool varint_pair_pack(VarintPair* pair, bool first, uint32_t duration);

uint8_t* varint_pair_get_data(VarintPair* pair);

size_t varint_pair_get_size(VarintPair* pair);

void varint_pair_reset(VarintPair* pair);

bool varint_pair_unpack(
    uint8_t* data,
    size_t data_length,
    uint32_t* value_1,
    uint32_t* value_2,
    size_t* length);

#ifdef __cplusplus
}
#endif