#include "subghz_protocol_blocks.h"

void subghz_protocol_blocks_add_bit(SubGhzProtocolBlockRuntime* runtime, uint8_t bit) {
    runtime->code_found = runtime->code_found << 1 | bit;
    runtime->code_count_bit++;
}

void subghz_protocol_blocks_set_decoder_callback(
    SubGhzProtocolBlockDecoder* block_decoder,
    SubGhzProtocolDecoderRxCallback callback,
    void* context) {
    block_decoder->callback = callback;
    block_decoder->context = context;
}

uint64_t subghz_protocol_blocks_reverse_key(uint64_t key, uint8_t count_bit) {
    uint64_t key_reverse = 0;
    for(uint8_t i = 0; i < count_bit; i++) {
        key_reverse = key_reverse << 1 | bit_read(key, i);
    }
    return key_reverse;
}