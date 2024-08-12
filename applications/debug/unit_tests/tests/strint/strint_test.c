#include <furi.h>
#include <furi_hal.h>

#include "../test.h" // IWYU pragma: keep

#include <toolbox/strint.h>

MU_TEST(strint_test_basic) {
    uint32_t result = 0;
    mu_assert_int_eq(StrintParseNoError, strint_to_uint32("123456", NULL, &result, 10));
    mu_assert_int_eq(123456, result);
}

MU_TEST(strint_test_junk) {
    uint32_t result = 0;
    mu_assert_int_eq(StrintParseNoError, strint_to_uint32("   123456   ", NULL, &result, 10));
    mu_assert_int_eq(123456, result);
    mu_assert_int_eq(
        StrintParseNoError, strint_to_uint32("   \r\n\r\n   123456     ", NULL, &result, 10));
    mu_assert_int_eq(123456, result);
}

MU_TEST(strint_test_tail) {
    uint32_t result = 0;
    char* tail;
    mu_assert_int_eq(StrintParseNoError, strint_to_uint32("123456tail", &tail, &result, 10));
    mu_assert_int_eq(123456, result);
    mu_assert_string_eq("tail", tail);
    mu_assert_int_eq(
        StrintParseNoError, strint_to_uint32("   \r\n  123456tail", &tail, &result, 10));
    mu_assert_int_eq(123456, result);
    mu_assert_string_eq("tail", tail);
}

MU_TEST(strint_test_errors) {
    uint32_t result = 123;
    mu_assert_int_eq(StrintParseAbsentError, strint_to_uint32("", NULL, &result, 10));
    mu_assert_int_eq(123, result);
    mu_assert_int_eq(StrintParseAbsentError, strint_to_uint32("   asd\r\n", NULL, &result, 10));
    mu_assert_int_eq(123, result);
    mu_assert_int_eq(StrintParseSignError, strint_to_uint32("+++123456", NULL, &result, 10));
    mu_assert_int_eq(123, result);
    mu_assert_int_eq(StrintParseSignError, strint_to_uint32("-1", NULL, &result, 10));
    mu_assert_int_eq(123, result);
    mu_assert_int_eq(
        StrintParseOverflowError,
        strint_to_uint32("0xAAAAAAAAAAAAAAAADEADBEEF!!!!!!", NULL, &result, 0));
    mu_assert_int_eq(123, result);
    mu_assert_int_eq(StrintParseOverflowError, strint_to_uint32("4294967296", NULL, &result, 0));
    mu_assert_int_eq(123, result);
}

MU_TEST(strint_test_bases) {
    uint32_t result = 0;

    mu_assert_int_eq(StrintParseNoError, strint_to_uint32("0x123", NULL, &result, 0));
    mu_assert_int_eq(0x123, result);
    mu_assert_int_eq(StrintParseNoError, strint_to_uint32("0X123", NULL, &result, 0));
    mu_assert_int_eq(0x123, result);
    mu_assert_int_eq(StrintParseNoError, strint_to_uint32("0xDEADBEEF", NULL, &result, 0));
    mu_assert_int_eq(0xDEADBEEF, result);
    mu_assert_int_eq(StrintParseNoError, strint_to_uint32("123", NULL, &result, 16));
    mu_assert_int_eq(0x123, result);

    mu_assert_int_eq(StrintParseNoError, strint_to_uint32("123", NULL, &result, 0));
    mu_assert_int_eq(123, result);

    mu_assert_int_eq(StrintParseNoError, strint_to_uint32("0123", NULL, &result, 0));
    mu_assert_int_eq(0123, result);
    mu_assert_int_eq(StrintParseNoError, strint_to_uint32("123", NULL, &result, 8));
    mu_assert_int_eq(0123, result);

    mu_assert_int_eq(StrintParseNoError, strint_to_uint32("0b101", NULL, &result, 0));
    mu_assert_int_eq(0b101, result);
    mu_assert_int_eq(StrintParseNoError, strint_to_uint32("0B101", NULL, &result, 0));
    mu_assert_int_eq(0b101, result);
    mu_assert_int_eq(StrintParseNoError, strint_to_uint32("101", NULL, &result, 2));
    mu_assert_int_eq(0b101, result);
}

MU_TEST_SUITE(strint_test_limits) {
    uint32_t result = 0;
    mu_assert_int_eq(StrintParseNoError, strint_to_uint32("4294967295", NULL, &result, 0));
    mu_assert_int_eq(UINT32_MAX, result);
    mu_assert_int_eq(StrintParseNoError, strint_to_uint32("0xFFFFFFFF", NULL, &result, 0));
    mu_assert_int_eq(UINT32_MAX, result);
    mu_assert_int_eq(
        StrintParseNoError,
        strint_to_uint32("0b11111111111111111111111111111111", NULL, &result, 0));
    mu_assert_int_eq(UINT32_MAX, result);
}

MU_TEST_SUITE(test_strint_suite) {
    MU_RUN_TEST(strint_test_basic);
    MU_RUN_TEST(strint_test_junk);
    MU_RUN_TEST(strint_test_tail);
    MU_RUN_TEST(strint_test_errors);
    MU_RUN_TEST(strint_test_bases);
    MU_RUN_TEST(strint_test_limits);
}

int run_minunit_test_strint(void) {
    MU_RUN_SUITE(test_strint_suite);
    return MU_EXIT_CODE;
}

TEST_API_DEFINE(run_minunit_test_strint)
