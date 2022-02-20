#include "generic.h"

#define TAG "SubGhzBlockGeneric"

bool subghz_block_generic_load_file(SubGhzBlockGeneric* instance, FlipperFormat* flipper_file) {
    furi_assert(instance);
    furi_assert(flipper_file);
    bool loaded = false;
    string_t temp_str;
    string_init(temp_str);
    uint32_t temp_data = 0;

    do {
        if(!flipper_format_read_uint32(flipper_file, "Bit", (uint32_t*)&temp_data, 1)) {
            FURI_LOG_E(TAG, "Missing Bit");
            break;
        }
        instance->data_count_bit = (uint8_t)temp_data;

        uint8_t key_data[sizeof(uint64_t)] = {0};
        if(!flipper_format_read_hex(flipper_file, "Key", key_data, sizeof(uint64_t))) {
            FURI_LOG_E(TAG, "Missing Key");
            break;
        }
        for(uint8_t i = 0; i < sizeof(uint64_t); i++) {
            instance->data = instance->data << 8 | key_data[i];
        }

        loaded = true;
    } while(0);

    string_clear(temp_str);

    return loaded;
}

bool subghz_block_generic_save_file(SubGhzBlockGeneric* instance, FlipperFormat* flipper_file) {
    furi_assert(instance);
    furi_assert(flipper_file);
    bool res = false;
    do {
        if(!flipper_format_write_string_cstr(flipper_file, "Protocol", instance->protocol_name)) {
            FURI_LOG_E(TAG, "Unable to add Protocol");
            break;
        }
        uint32_t temp = instance->data_count_bit;
        if(!flipper_format_write_uint32(flipper_file, "Bit", &temp, 1)) {
            FURI_LOG_E(TAG, "Unable to add Bit");
            break;
        }

        uint8_t key_data[sizeof(uint64_t)] = {0};
        for(size_t i = 0; i < sizeof(uint64_t); i++) {
            key_data[sizeof(uint64_t) - i - 1] = (instance->data >> i * 8) & 0xFF;
        }

        if(!flipper_format_write_hex(flipper_file, "Key", key_data, sizeof(uint64_t))) {
            FURI_LOG_E(TAG, "Unable to add Key");
            break;
        }
        res = true;
    } while(false);

    return res;
}
