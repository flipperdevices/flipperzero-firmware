#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#define BER_CLASS_UNIVERSAL 0x0
#define BER_CLASS_APPLICATION 0x1
#define BER_CLASS_CONTEXT 0x2
#define BER_CLASS_PRIVATE 0x3

typedef struct {
    union {
        uint16_t tag;
        struct {
            // LSB
            uint8_t tag : 5;
            uint8_t constructed : 1;
            uint8_t class : 2;
            // MSB
        } ber;
        //TODO: currently only works for 1-byte tags
    };
	size_t length;
	const uint8_t* value;

	const uint8_t* next;
} TlvInfo;

// ISO7816-5 §5.2
// Simple-TLV and BER-TLV parsing
TlvInfo iso7816_tlv_parse(const uint8_t* data);
