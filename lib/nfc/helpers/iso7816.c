#include "iso7816.h"

// ISO7816-5
// Simple-TLV (§5.2.1)
// BER-TLV (§5.2.2)
TlvInfo iso7816_tlv_parse(const uint8_t* data) {
	TlvInfo tlv;

	// Simple-TLV: tag can be any value from 1 to 254 (not '00' or 'FF')
	// BER-TLV: TODO describe
	// 00000 - 11110 => 0 - 30 (single byte)
	// 11111 00011111 - 11111 01111111 => 31 - 127 (2 byte)
	// 11111 10000001 00000001 - 11111 11111111 01111111 => 128 - 16383 (3 byte)

	tlv.tag = *(data++);
	if ((tlv.tag & 0x1f) == 0x1f) {
		// BER-TLV, multi byte tag
		tlv.tag = *(data++);
		if(tlv.tag & 0x80) {
			// BER-TLV, 3 byte tag
			tlv.tag &= ~0x80;
			tlv.tag <<= 7;
			tlv.tag |= *(data++) & 0x7f;
		}
	}

	//TODO: check for invalid 'indefinite length'
	tlv.length = *(data++);
	if (tlv.length == 0xff) {
		// Simple-TLV 2 byte length
		tlv.length = *(data++) << 8;
		tlv.length += *(data++);
	} else if(tlv.length > 0x7f) {
		uint8_t length_bytes = tlv.length & 0x7f;
		//printf("BER length of %d bytes\n", length_bytes);
		if (length_bytes < 1 || length_bytes > 4) {
			//TODO: error: ISO7816 doesn't support more than 4 length bytes
			return (TlvInfo){.tag = 0};
		}
		tlv.length = 0;
		for(uint8_t i=0; i<length_bytes; ++i) {
			//printf("byte %d: %02x\n", i, *data);
			tlv.length <<= 8;
			tlv.length |= *(data++);
		}
	}
	tlv.value = data;
	tlv.next = data + tlv.length;

	return tlv;
}
