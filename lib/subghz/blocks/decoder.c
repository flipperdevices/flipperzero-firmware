#include "decoder.h"

#define TAG "SubGhzBlockDecoder"

void subghz_protocol_blocks_add_bit(SubGhzBlockDecoder* decoder, uint8_t bit) {
    decoder->decode_data = decoder->decode_data << 1 | bit;
    decoder->decode_count_bit++;
}
