#include "EM41XX.h"
#include "core/check.h"
#include "core/log.h"

#define EM41XX_LINES (10)
#define EM41XX_COLUMNS (4)

#define EM41XX_CONFIG_1_KEY (0b00000000000101001000000001000000)
#define EM41XX_CONFIG_2_KEYS (0b00000000000101001000000010000000)
#define EM41XX_CONFIG_3_KEYS (0b00000000000101001000000011000000)
#define EM41XX_CONFIG_BLANK (0b00000000000101001000000000000000)

#define EM41XX_HEADER (0b111111111)

#define T5577_MAX_BLOCKS (8)
#define EM41XX_BLOCKS (2)

bool get_parity(uint16_t data) {
    bool result = 0;
    for(int i = 0; i < 16; i++) result ^= ((data >> i) & 1);
    return result;
}

bool get_line_parity_bit(uint8_t line_num, uint64_t data) {
    uint8_t line = (data >> (EM41XX_COLUMNS * line_num)) & 0x0F;
    return get_parity(line);
}

bool get_column_parity_bit(uint8_t column_num, uint64_t data) {
    uint16_t column = 0;

    for(int i = 0; i < EM41XX_LINES; i++) {
        column <<= 1;
        column |= (data >> (EM41XX_COLUMNS * i + column_num)) & 1;
    }

    return get_parity(column);
}

uint64_t em41xx_encode(uint64_t data) {
    uint64_t result = EM41XX_HEADER;

    for(int i = EM41XX_LINES - 1; i >= 0; i--) {
        result <<= EM41XX_COLUMNS;
        uint8_t line = (data >> (i * EM41XX_COLUMNS)) & 0x0F;
        result |= line;

        result <<= 1;
        result |= get_line_parity_bit(i, data);
    }

    for(int i = EM41XX_COLUMNS - 1; i >= 0; i--) {
        result <<= 1;
        result |= get_column_parity_bit(i, data);
    }

    result <<= 1;

    return result;
}

bool add_em41xx_data(LFRFIDT5577* data, uint64_t key, uint8_t from_index) {
    if(from_index + EM41XX_BLOCKS > (T5577_MAX_BLOCKS - 1)) return false;

    uint64_t blocks_data = em41xx_encode(key);
    data->block[from_index] = blocks_data >> 32;
    data->block[from_index + 1] = blocks_data & 0xFFFFFFFF;
    data->blocks_to_write = T5577_MAX_BLOCKS;

    uint8_t mask_addition = (1 << from_index);
    mask_addition |= (1 << (from_index + 1));

    data->mask |= mask_addition;

    FURI_LOG_D("ADDDATA", "%u", data->mask);

    return true;
}

uint32_t get_config(uint8_t keys_count) {
    if(keys_count > 3) return 0;

    uint32_t result = EM41XX_CONFIG_BLANK;
    result |= ((keys_count * EM41XX_BLOCKS) << 5);

    return result;
}

bool set_em41xx_config(LFRFIDT5577* data, uint8_t keys_count) {
    if(keys_count > 3) return false;

    data->block[0] = get_config(keys_count);

    data->mask |= 1;
    FURI_LOG_D("SETCONFIG", "%u", data->mask);

    return true;
}

uint64_t bytes2num(const uint8_t* src, uint8_t len) {
    furi_assert(src);
    furi_assert(len <= 8);

    uint64_t res = 0;
    while(len--) {
        res = (res << 8) | (*src);
        src++;
    }
    return res;
}
