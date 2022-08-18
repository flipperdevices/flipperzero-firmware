#include "varint_pair.h"
#include <toolbox/varint.h>

#define VARINT_PAIR_SIZE 10

struct VarintPair {
    size_t data_length;
    uint8_t data[VARINT_PAIR_SIZE];
};

VarintPair* varint_pair_alloc() {
    VarintPair* pair = malloc(sizeof(VarintPair));
    pair->data_length = 0;
    return pair;
}

void varint_pair_free(VarintPair* pair) {
    free(pair);
}

bool varint_pair_pack(VarintPair* pair, bool first, uint32_t duration) {
    bool result = false;

    if(first) {
        if(pair->data_length == 0) {
            pair->data_length = varint_uint32_pack(duration, pair->data);
        } else {
            pair->data_length = 0;
        }
    } else {
        if(pair->data_length > 0) {
            pair->data_length += varint_uint32_pack(duration, pair->data + pair->data_length);
            result = true;
        } else {
            pair->data_length = 0;
        }
    }

    return result;
}

uint8_t* varint_pair_get_data(VarintPair* pair) {
    return pair->data;
}

size_t varint_pair_get_size(VarintPair* pair) {
    return pair->data_length;
}

void varint_pair_reset(VarintPair* pair) {
    pair->data_length = 0;
}
