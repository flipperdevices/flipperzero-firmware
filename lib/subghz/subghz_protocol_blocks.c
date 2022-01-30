#include "subghz_protocol_blocks.h"

#define SUBGHZ_PARSER_TAG "SubGhzProtocolBlock"

void subghz_protocol_blocks_add_bit(SubGhzProtocolBlockDecoder* decoder, uint8_t bit) {
    decoder->decode_data = decoder->decode_data << 1 | bit;
    decoder->decode_count_bit++;
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

bool subghz_protocol_block_save_file(SubGhzProtocolBlockGeneric* instance, FlipperFile* flipper_file) {
    furi_assert(instance);
    furi_assert(flipper_file);
    bool res = false;
    do {
        if(!flipper_file_write_string_cstr(flipper_file, "Protocol", instance->protocol_name)) {
            FURI_LOG_E(SUBGHZ_PARSER_TAG, "Unable to add Protocol");
            break;
        }
        uint32_t temp = instance->data_count_bit;
        if(!flipper_file_write_uint32(flipper_file, "Bit", &temp, 1)) {
            FURI_LOG_E(SUBGHZ_PARSER_TAG, "Unable to add Bit");
            break;
        }

        uint8_t key_data[sizeof(uint64_t)] = {0};
        for(size_t i = 0; i < sizeof(uint64_t); i++) {
            key_data[sizeof(uint64_t) - i - 1] = (instance->data >> i * 8) & 0xFF;
        }

        if(!flipper_file_write_hex(flipper_file, "Key", key_data, sizeof(uint64_t))) {
            FURI_LOG_E(SUBGHZ_PARSER_TAG, "Unable to add Key");
            break;
        }
        res = true;
    } while(false);

    return res;
}

bool subghz_protocol_block_load_file(
    SubGhzProtocolBlockGeneric* instance,
    FlipperFile* flipper_file) {
    furi_assert(instance);
    furi_assert(flipper_file);
    bool loaded = false;
    string_t temp_str;
    string_init(temp_str);
    uint32_t temp_data = 0;

    do {
        if(!flipper_file_read_uint32(flipper_file, "Bit", (uint32_t*)&temp_data, 1)) {
            FURI_LOG_E(SUBGHZ_PARSER_TAG, "Missing Bit");
            break;
        }
        instance->data_count_bit = (uint8_t)temp_data;

        uint8_t key_data[sizeof(uint64_t)] = {0};
        if(!flipper_file_read_hex(flipper_file, "Key", key_data, sizeof(uint64_t))) {
            FURI_LOG_E(SUBGHZ_PARSER_TAG, "Missing Key");
            break;
        }
        for(uint8_t i = 0; i < sizeof(uint64_t); i++) {
            instance->data= instance->data << 8 | key_data[i];
        }

        loaded = true;
    } while(0);

    string_clear(temp_str);

    return loaded;
}
