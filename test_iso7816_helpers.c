#include <stdio.h>

#include "lib/nfc/helpers/iso7816.h"

#define COLOR_RED "\033[0;31m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RESET "\033[0;0m"

void print_hex(const uint8_t* data, size_t length) {
    for(size_t i=0; i<length; ++i) {
        printf("%02X", data[i]);
    }
}

void test_iso7816_tlv_parse(const uint8_t* input, size_t input_size, uint16_t exp_tag, size_t exp_length) {
	TlvInfo tlv = iso7816_tlv_parse(input);

	if(tlv.tag != exp_tag) {
        printf(COLOR_RED "FAILED  - iso7816_tlv_parse Tag for ");
		print_hex(input, input_size);
		printf(" is not %d, but %d\n" COLOR_RESET,
                exp_tag, tlv.tag);
        return;
    }

	if(tlv.length != exp_length) {
        printf(COLOR_RED "FAILED  - iso7816_tlv_parse Length for ");
		print_hex(input, input_size);
		printf(" is not %ld, but %ld\n" COLOR_RESET,
                exp_length, tlv.length);
        return;
    }

    printf(COLOR_GREEN "SUCCESS - iso7816_tlv_parse for ");
	print_hex(input, input_size);
	printf(" is tag:%d, length:%ld\n" COLOR_RESET, tlv.tag, tlv.length);
}

void test_iso7816_tlv_parse_ber(const uint8_t* input, size_t input_size, uint8_t exp_class, uint8_t exp_constructed, uint16_t exp_tag, size_t exp_length) {
	TlvInfo tlv = iso7816_tlv_parse(input);

	if(tlv.ber.class != exp_class) {
        printf(COLOR_RED "FAILED  - iso7816_tlv_parse ber.class for ");
		print_hex(input, input_size);
		printf(" is not %d, but %d\n" COLOR_RESET,
                exp_class, tlv.ber.class);
        return;
    }

	if(tlv.ber.constructed != exp_constructed) {
        printf(COLOR_RED "FAILED  - iso7816_tlv_parse ber.constructed for ");
		print_hex(input, input_size);
		printf(" is not %d, but %d\n" COLOR_RESET,
                exp_constructed, tlv.ber.constructed);
        return;
    }

	if(tlv.ber.tag != exp_tag) {
        printf(COLOR_RED "FAILED  - iso7816_tlv_parse ber.tag for ");
		print_hex(input, input_size);
		printf(" is not %d, but %d\n" COLOR_RESET,
                exp_tag, tlv.ber.tag);
        return;
    }

	if(tlv.length != exp_length) {
        printf(COLOR_RED "FAILED  - iso7816_tlv_parse length for ");
		print_hex(input, input_size);
		printf(" is not %ld, but %ld\n" COLOR_RESET,
                exp_length, tlv.length);
        return;
    }

    printf(COLOR_GREEN "SUCCESS - iso7816_tlv_parse BER for ");
	print_hex(input, input_size);
	printf(" is class:%d, constructed:%d, tag:%d, length:%ld\n" COLOR_RESET, tlv.ber.class, tlv.ber.constructed, tlv.ber.tag, tlv.length);
}

void describe_tlv(const uint8_t* data, size_t length, size_t indent) {
    TlvInfo tlv = iso7816_tlv_parse(data);

    char prefix[indent+1];
    memset(prefix, ' ', indent);
    prefix[indent] = '\x00';

    printf("%sTag: %d (BER - class: %d, constr: %d, tag: %d)\n", prefix, tlv.tag, tlv.ber.class, tlv.ber.constructed, tlv.ber.tag);
    printf("%sLength: %ld\n", prefix, tlv.length);
    printf("%sValue: ", prefix);
    print_hex(tlv.value, tlv.length);
    printf("\n");

    if(tlv.ber.constructed) {
        describe_tlv(tlv.value, tlv.length, indent+2);
    }
}

int main(int argc, char** argv) {
	test_iso7816_tlv_parse("\x0F\x05\x48\x65\x6C\x6C\x6F", 7, 15, 5);
	test_iso7816_tlv_parse("\x5F\x0F\x05\x48\x65\x6C\x6C\x6F", 8, 15, 5);
	test_iso7816_tlv_parse("\x5F\x1F\x05\x48\x65\x6C\x6C\x6F", 8, 31, 5);
	test_iso7816_tlv_parse("\x5F\x7F\x05\x48\x65\x6C\x6C\x6F", 8, 127, 5);
	test_iso7816_tlv_parse("\x5F\x81\x00\x05\x48\x65\x6C\x6C\x6F", 9, 128, 5);
	test_iso7816_tlv_parse("\x5F\xFF\x7F\x05\x48\x65\x6C\x6C\x6F", 9, 16383, 5);
	test_iso7816_tlv_parse("\x0F\xff\x00\x05\x48\x65\x6C\x6C\x6F", 9, 15, 5);
	test_iso7816_tlv_parse("\x04\xff\x01\x00\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65\x65", 260, 4, 256);
	test_iso7816_tlv_parse("\x4F\x81\x05\x48\x65\x6C\x6C\x6F", 8, 0x4f, 5);
	test_iso7816_tlv_parse("\x4F\x82\x00\x05\x48\x65\x6C\x6C\x6F", 9, 0x4f, 5);
	test_iso7816_tlv_parse("\x4F\x83\x00\x00\x05\x48\x65\x6C\x6C\x6F", 10, 0x4f, 5);
	test_iso7816_tlv_parse("\x4F\x84\x00\x00\x00\x05\x48\x65\x6C\x6C\x6F", 11, 0x4f, 5);
	test_iso7816_tlv_parse("\x4F\x85\x00\x00\x00\x00\x05\x48\x65\x6C\x6C\x6F", 12, 0, 0);
    test_iso7816_tlv_parse("\x61\x09\x4F\x07\xA0\x00\x00\x02\x47\x10\x01\x90\x00", 13, 97, 9);

    test_iso7816_tlv_parse_ber("\x8A\x02Hi", 4, BER_CLASS_CONTEXT, 0, 10, 2);
    test_iso7816_tlv_parse_ber("\x6A\x04\x8A\x02Hi", 6, BER_CLASS_APPLICATION, 1, 10, 4);

    const uint8_t *ef_dir_data = "\x61\x09\x4F\x07\xA0\x00\x00\x02\x47\x10\x01\x61\x09\x4F\x07\xA0\x00\x00\x02\x47\x20\x01";
    size_t ef_dir_data_len = 22;
    describe_tlv(ef_dir_data, ef_dir_data_len, 0);

    const uint8_t *ef_cardaccess_data = "\x31\x14\x30\x12\x06\x0A\x04\x00\x7F\x00\x07\x02\x02\x04\x02\x04\x02\x01\x02\x02\x01\x0E\x90\x00";
    size_t ef_cardaccess_data_len = 24;
    describe_tlv(ef_cardaccess_data, ef_cardaccess_data_len, 0);

	return 0;
}
