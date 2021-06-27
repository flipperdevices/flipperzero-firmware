#pragma once

#include <api-hal.h>
#include <stdint.h>

#define bit_read(value, bit) (((value) >> (bit)) & 0x01)
#define bit_set(value, bit) ((value) |= (1UL << (bit)))
#define bit_clear(value, bit) ((value) &= ~(1UL << (bit)))
#define bit_write(value, bit, bitvalue) (bitvalue ? bit_set(value, bit) : bit_clear(value, bit))

#define SUBGHZ_TX_PIN_HIGTH() 
#define SUBGHZ_TX_PIN_LOW() 
#define DURATION_DIFF(x,y) ((x < y) ? (y - x) : (x - y))

#define SUBGHZ_PROTOCOL_NAME_LEN 15
typedef struct{
    char        name [SUBGHZ_PROTOCOL_NAME_LEN];
    char        name_remote_controller [SUBGHZ_PROTOCOL_NAME_LEN];
    uint16_t    te_long;
    uint16_t    te_shot;
    uint16_t    te_delta;
    uint64_t    code_found;
    uint64_t    code_last_found;
    uint8_t     code_count_bit;
    uint8_t     code_min_count_bit_for_found;
    uint8_t     parser_step;
    uint16_t    te_last;
    uint8_t     header_count;
    uint16_t    cnt;
} SubGhzProtocolCommon;

void subghz_protocol_common_add_bit(SubGhzProtocolCommon *common, uint8_t bit);

uint8_t subghz_protocol_common_check_interval(SubGhzProtocolCommon *common, uint32_t interval, uint16_t interval_check);

void subghz_protocol_common_printf(SubGhzProtocolCommon *common);

uint64_t subghz_protocol_common_reverse_key(uint64_t key, uint8_t count_bit);
