#pragma once

#include <stdint.h>

extern const char hex_asc_upper[];

#define hex_asc_upper_lo(x) hex_asc_upper[((x)&0x0F)]
#define hex_asc_upper_hi(x) hex_asc_upper[((x)&0xF0) >> 4]

#define put_sff_id(buf, id) _put_id(buf, 2, id)
#define put_eff_id(buf, id) _put_id(buf, 7, id)

void _put_id(uint8_t* buf, int end_offset, uint32_t id);

void put_hex_byte(uint8_t* buf, uint8_t byte);

unsigned char hexCharToByte(char hex);

uint8_t ascii2byte(uint8_t* val);

uint8_t nibble2ascii(uint8_t nibble);
