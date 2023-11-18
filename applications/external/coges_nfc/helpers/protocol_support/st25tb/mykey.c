#include "mykey.h"

uint32_t encode_decode_block(uint32_t input) {
    /*
     * Swap all values using XOR
     * 32 bit: 1111222233334444
     */
    input ^= (input & 0x00C00000) << 6 | (input & 0x0000C000) << 12 | (input & 0x000000C0) << 18 |
             (input & 0x000C0000) >> 6 | (input & 0x00030000) >> 12 | (input & 0x00000300) >> 6;
    input ^= (input & 0x30000000) >> 6 | (input & 0x0C000000) >> 12 | (input & 0x03000000) >> 18 |
             (input & 0x00003000) << 6 | (input & 0x00000030) << 12 | (input & 0x0000000C) << 6;
    input ^= (input & 0x00C00000) << 6 | (input & 0x0000C000) << 12 | (input & 0x000000C0) << 18 |
             (input & 0x000C0000) >> 6 | (input & 0x00030000) >> 12 | (input & 0x00000300) >> 6;
    return input;
}

/* Bytewise LITTLE ENDIAN */
uint32_t st25tb_get_block_value(uint32_t block) {
    uint8_t byte0 = (block >> 24) & 0xFF;
    uint8_t byte1 = (block >> 16) & 0xFF;
    uint8_t byte2 = (block >> 8) & 0xFF;
    uint8_t byte3 = block & 0xFF;

    return (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0;
}

//Ottengo il blocco effetto un bytewise e lo decodo
uint32_t get_block(uint32_t block) {
    return encode_decode_block(st25tb_get_block_value(block));
}

//decode for credit
uint32_t get_xored_block(uint32_t block, uint32_t key) {
    return encode_decode_block(st25tb_get_block_value(block) ^ key);
}