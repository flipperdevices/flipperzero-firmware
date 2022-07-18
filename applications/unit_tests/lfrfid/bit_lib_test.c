#include <furi.h>
#include "../minunit.h"
#include <lfrfid/tools/bit_lib.h>

MU_TEST(test_bit_lib_increment_index) {
    uint32_t index = 0;

    // test increment
    for(uint32_t i = 0; i < 31; ++i) {
        bit_lib_increment_index(index, 32);
        mu_assert_int_eq(i + 1, index);
    }

    // test wrap around
    for(uint32_t i = 0; i < 512; ++i) {
        bit_lib_increment_index(index, 32);
        mu_assert_int_less_than(32, index);
    }
}

MU_TEST(test_bit_lib_is_set) {
    uint32_t value = 0x0000FFFF;

    for(uint32_t i = 0; i < 16; ++i) {
        mu_check(bit_lib_bit_is_set(value, i));
        mu_check(!bit_lib_bit_is_not_set(value, i));
    }

    for(uint32_t i = 16; i < 32; ++i) {
        mu_check(!bit_lib_bit_is_set(value, i));
        mu_check(bit_lib_bit_is_not_set(value, i));
    }
}

MU_TEST(test_bit_lib_push) {
#define TEST_BIT_LIB_PUSH_DATA_SIZE 4
    uint8_t data[TEST_BIT_LIB_PUSH_DATA_SIZE] = {0};
    uint8_t expected_data_1[TEST_BIT_LIB_PUSH_DATA_SIZE] = {0x00, 0x00, 0x0F, 0xFF};
    uint8_t expected_data_2[TEST_BIT_LIB_PUSH_DATA_SIZE] = {0x00, 0xFF, 0xF0, 0x00};
    uint8_t expected_data_3[TEST_BIT_LIB_PUSH_DATA_SIZE] = {0xFF, 0x00, 0x00, 0xFF};
    uint8_t expected_data_4[TEST_BIT_LIB_PUSH_DATA_SIZE] = {0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t expected_data_5[TEST_BIT_LIB_PUSH_DATA_SIZE] = {0x00, 0x00, 0x00, 0x00};
    uint8_t expected_data_6[TEST_BIT_LIB_PUSH_DATA_SIZE] = {0xCC, 0xCC, 0xCC, 0xCC};

    for(uint32_t i = 0; i < 12; ++i) {
        bit_lib_push_bit(data, TEST_BIT_LIB_PUSH_DATA_SIZE, true);
    }
    mu_assert_mem_eq(expected_data_1, data, TEST_BIT_LIB_PUSH_DATA_SIZE);

    for(uint32_t i = 0; i < 12; ++i) {
        bit_lib_push_bit(data, TEST_BIT_LIB_PUSH_DATA_SIZE, false);
    }
    mu_assert_mem_eq(expected_data_2, data, TEST_BIT_LIB_PUSH_DATA_SIZE);

    for(uint32_t i = 0; i < 4; ++i) {
        bit_lib_push_bit(data, TEST_BIT_LIB_PUSH_DATA_SIZE, false);
    }
    for(uint32_t i = 0; i < 8; ++i) {
        bit_lib_push_bit(data, TEST_BIT_LIB_PUSH_DATA_SIZE, true);
    }
    mu_assert_mem_eq(expected_data_3, data, TEST_BIT_LIB_PUSH_DATA_SIZE);

    for(uint32_t i = 0; i < TEST_BIT_LIB_PUSH_DATA_SIZE * 8; ++i) {
        bit_lib_push_bit(data, TEST_BIT_LIB_PUSH_DATA_SIZE, true);
    }
    mu_assert_mem_eq(expected_data_4, data, TEST_BIT_LIB_PUSH_DATA_SIZE);

    for(uint32_t i = 0; i < TEST_BIT_LIB_PUSH_DATA_SIZE * 8; ++i) {
        bit_lib_push_bit(data, TEST_BIT_LIB_PUSH_DATA_SIZE, false);
    }
    mu_assert_mem_eq(expected_data_5, data, TEST_BIT_LIB_PUSH_DATA_SIZE);

    for(uint32_t i = 0; i < TEST_BIT_LIB_PUSH_DATA_SIZE * 2; ++i) {
        bit_lib_push_bit(data, TEST_BIT_LIB_PUSH_DATA_SIZE, true);
        bit_lib_push_bit(data, TEST_BIT_LIB_PUSH_DATA_SIZE, true);
        bit_lib_push_bit(data, TEST_BIT_LIB_PUSH_DATA_SIZE, false);
        bit_lib_push_bit(data, TEST_BIT_LIB_PUSH_DATA_SIZE, false);
    }
    mu_assert_mem_eq(expected_data_6, data, TEST_BIT_LIB_PUSH_DATA_SIZE);
}

MU_TEST(test_bit_lib_set_bit) {
    uint8_t value[2] = {0x00, 0xFF};
    bit_lib_set_bit(value, 15, false);
    mu_assert_mem_eq(value, ((uint8_t[]){0x00, 0xFE}), 2);
    bit_lib_set_bit(value, 14, false);
    mu_assert_mem_eq(value, ((uint8_t[]){0x00, 0xFC}), 2);
    bit_lib_set_bit(value, 13, false);
    mu_assert_mem_eq(value, ((uint8_t[]){0x00, 0xF8}), 2);
    bit_lib_set_bit(value, 12, false);
    mu_assert_mem_eq(value, ((uint8_t[]){0x00, 0xF0}), 2);
    bit_lib_set_bit(value, 11, false);
    mu_assert_mem_eq(value, ((uint8_t[]){0x00, 0xE0}), 2);
    bit_lib_set_bit(value, 10, false);
    mu_assert_mem_eq(value, ((uint8_t[]){0x00, 0xC0}), 2);
    bit_lib_set_bit(value, 9, false);
    mu_assert_mem_eq(value, ((uint8_t[]){0x00, 0x80}), 2);
    bit_lib_set_bit(value, 8, false);
    mu_assert_mem_eq(value, ((uint8_t[]){0x00, 0x00}), 2);

    bit_lib_set_bit(value, 7, true);
    mu_assert_mem_eq(value, ((uint8_t[]){0x01, 0x00}), 2);
    bit_lib_set_bit(value, 6, true);
    mu_assert_mem_eq(value, ((uint8_t[]){0x03, 0x00}), 2);
    bit_lib_set_bit(value, 5, true);
    mu_assert_mem_eq(value, ((uint8_t[]){0x07, 0x00}), 2);
    bit_lib_set_bit(value, 4, true);
    mu_assert_mem_eq(value, ((uint8_t[]){0x0F, 0x00}), 2);
    bit_lib_set_bit(value, 3, true);
    mu_assert_mem_eq(value, ((uint8_t[]){0x1F, 0x00}), 2);
    bit_lib_set_bit(value, 2, true);
    mu_assert_mem_eq(value, ((uint8_t[]){0x3F, 0x00}), 2);
    bit_lib_set_bit(value, 1, true);
    mu_assert_mem_eq(value, ((uint8_t[]){0x7F, 0x00}), 2);
    bit_lib_set_bit(value, 0, true);
    mu_assert_mem_eq(value, ((uint8_t[]){0xFF, 0x00}), 2);
}

MU_TEST(test_bit_lib_set_bits) {
    uint8_t value[2] = {0b00000000, 0b11111111};
    // set 4 bits to 0b0100 from 12 index
    bit_lib_set_bits(value, 12, 0b0100, 4);
    //                                                    [0100]
    mu_assert_mem_eq(value, ((uint8_t[]){0b00000000, 0b11110100}), 2);

    // set 2 bits to 0b11 from 11 index
    bit_lib_set_bits(value, 11, 0b11, 2);
    //                                                    [11]
    mu_assert_mem_eq(value, ((uint8_t[]){0b00000000, 0b11111100}), 2);

    // set 3 bits to 0b111 from 0 index
    bit_lib_set_bits(value, 0, 0b111, 3);
    //                                    [111]
    mu_assert_mem_eq(value, ((uint8_t[]){0b11100000, 0b11111100}), 2);

    // set 8 bits to 0b11111000 from 3 index
    bit_lib_set_bits(value, 3, 0b11111000, 8);
    //                                       [11111    000]
    mu_assert_mem_eq(value, ((uint8_t[]){0b11111111, 0b00011100}), 2);
}

MU_TEST(test_bit_lib_get_bit) {
    uint8_t value[2] = {0b00000000, 0b11111111};
    for(uint32_t i = 0; i < 8; ++i) {
        mu_check(bit_lib_get_bit(value, i) == false);
    }
    for(uint32_t i = 8; i < 16; ++i) {
        mu_check(bit_lib_get_bit(value, i) == true);
    }
}

MU_TEST(test_bit_lib_get_bits) {
    uint8_t value[2] = {0b00000000, 0b11111111};
    mu_assert_int_eq(bit_lib_get_bits(value, 0, 8), 0b00000000);
    mu_assert_int_eq(bit_lib_get_bits(value, 1, 8), 0b00000001);
    mu_assert_int_eq(bit_lib_get_bits(value, 2, 8), 0b00000011);
    mu_assert_int_eq(bit_lib_get_bits(value, 3, 8), 0b00000111);
    mu_assert_int_eq(bit_lib_get_bits(value, 4, 8), 0b00001111);
    mu_assert_int_eq(bit_lib_get_bits(value, 5, 8), 0b00011111);
    mu_assert_int_eq(bit_lib_get_bits(value, 6, 8), 0b00111111);
    mu_assert_int_eq(bit_lib_get_bits(value, 7, 8), 0b01111111);
    mu_assert_int_eq(bit_lib_get_bits(value, 8, 8), 0b11111111);
}

MU_TEST(test_bit_lib_test_parity_u32) {
    // test even parity
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00000000, BitLibParityEven), 0);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00000001, BitLibParityEven), 1);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00000010, BitLibParityEven), 1);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00000011, BitLibParityEven), 0);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00000100, BitLibParityEven), 1);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00000101, BitLibParityEven), 0);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00000110, BitLibParityEven), 0);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00000111, BitLibParityEven), 1);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00001000, BitLibParityEven), 1);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00001001, BitLibParityEven), 0);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00001010, BitLibParityEven), 0);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00001011, BitLibParityEven), 1);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00001100, BitLibParityEven), 0);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00001101, BitLibParityEven), 1);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00001110, BitLibParityEven), 1);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00001111, BitLibParityEven), 0);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00010000, BitLibParityEven), 1);

    // test odd parity
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00000000, BitLibParityOdd), 1);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00000001, BitLibParityOdd), 0);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00000010, BitLibParityOdd), 0);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00000011, BitLibParityOdd), 1);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00000100, BitLibParityOdd), 0);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00000101, BitLibParityOdd), 1);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00000110, BitLibParityOdd), 1);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00000111, BitLibParityOdd), 0);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00001000, BitLibParityOdd), 0);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00001001, BitLibParityOdd), 1);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00001010, BitLibParityOdd), 1);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00001011, BitLibParityOdd), 0);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00001100, BitLibParityOdd), 1);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00001101, BitLibParityOdd), 0);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00001110, BitLibParityOdd), 0);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00001111, BitLibParityOdd), 1);
    mu_assert_int_eq(bit_lib_test_parity_u32(0b00010000, BitLibParityOdd), 0);
}

MU_TEST_SUITE(test_bit_lib) {
    MU_RUN_TEST(test_bit_lib_increment_index);
    MU_RUN_TEST(test_bit_lib_is_set);
    MU_RUN_TEST(test_bit_lib_push);
    MU_RUN_TEST(test_bit_lib_set_bit);
    MU_RUN_TEST(test_bit_lib_set_bits);
    MU_RUN_TEST(test_bit_lib_get_bit);
    MU_RUN_TEST(test_bit_lib_get_bits);
    MU_RUN_TEST(test_bit_lib_test_parity_u32);
}

int run_minunit_test_bit_lib() {
    MU_RUN_SUITE(test_bit_lib);
    return MU_EXIT_CODE;
}