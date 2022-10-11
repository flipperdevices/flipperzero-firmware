#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

typedef struct {
	uint16_t tag;
	size_t length;
	const uint8_t* value;

	const uint8_t* next;
} TlvInfo;

// ISO7816-5 §5.2
// Simple-TLV and BER-TLV parsing
TlvInfo iso7816_tlv_parse(const uint8_t* data);
