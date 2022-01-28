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