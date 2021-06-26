#pragma once

#include <api-hal.h>
#include <api-hal-subghz.h>

#define bit_read(value, bit) (((value) >> (bit)) & 0x01)
#define bit_set(value, bit) ((value) |= (1UL << (bit)))
#define bit_clear(value, bit) ((value) &= ~(1UL << (bit)))
#define bit_write(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#define SUBGHZ_TX_PIN_HIGTH() 
#define SUBGHZ_TX_PIN_LOW() 
#define DURATION_DIFF(x,y) ((x<y)?(y-x):(x-y))

#define CC1101_PROTOCOL_NAME_LEN 15
typedef struct{
    char        name [CC1101_PROTOCOL_NAME_LEN];
    char        name_remote_controller [CC1101_PROTOCOL_NAME_LEN];
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
} CC1101ProtocolStruct;

void subghz_protocol_header_add_bit(CC1101ProtocolStruct *pr, uint8_t bit);
uint8_t subghz_protocol_header_check_interval (CC1101ProtocolStruct *pr,uint32_t interval, uint16_t intervel_check);
void subghz_protocol_header_printf (CC1101ProtocolStruct *pr);
uint64_t subghz_protocol_header_reverse_key(uint64_t key, uint8_t count_bit);
