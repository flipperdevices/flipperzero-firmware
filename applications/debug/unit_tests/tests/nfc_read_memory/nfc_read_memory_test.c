#include <furi.h>

#include "../test.h" // IWYU pragma: keep

#include <applications/main/nfc/helpers/nfc_read_memory.h>

// Heap states measured on device: free heap, largest free block
#define CRASH_FREE_HEAP      (8944)
#define QFLIPPER_FREE_HEAP   (9416)
#define QFLIPPER_MAX_BLOCK   (8896)
#define MODERATE_FREE_HEAP   (18384)
#define MODERATE_MAX_BLOCK   (17232)
#define IDLE_FREE_HEAP       (30664)
#define IDLE_MAX_BLOCK       (30600)
#define FRAGMENTED_MAX_BLOCK (4 * 1024)

MU_TEST(nfc_read_memory_required_test) {
    mu_assert_int_eq(NFC_READ_MEMORY_BASE_FREE_HEAP, nfc_read_memory_required(NfcProtocolInvalid));
    mu_assert_int_eq(
        NFC_READ_MEMORY_BASE_FREE_HEAP, nfc_read_memory_required(NfcProtocolIso14443_4a));
    mu_assert_int_eq(
        NFC_READ_MEMORY_BASE_FREE_HEAP + 2 * sizeof(MfClassicData),
        nfc_read_memory_required(NfcProtocolMfClassic));
    mu_assert_int_eq(
        NFC_READ_MEMORY_BASE_FREE_HEAP + 2 * sizeof(MfUltralightData),
        nfc_read_memory_required(NfcProtocolMfUltralight));
}

MU_TEST(nfc_read_memory_low_heap_test) {
    mu_check(!nfc_read_memory_is_enough(NfcProtocolMfClassic, CRASH_FREE_HEAP, CRASH_FREE_HEAP));
    mu_check(
        !nfc_read_memory_is_enough(NfcProtocolInvalid, QFLIPPER_FREE_HEAP, QFLIPPER_MAX_BLOCK));
    for(NfcProtocol protocol = 0; protocol < NfcProtocolNum; protocol++) {
        mu_check(!nfc_read_memory_is_enough(protocol, QFLIPPER_FREE_HEAP, QFLIPPER_MAX_BLOCK));
    }
}

MU_TEST(nfc_read_memory_moderate_heap_test) {
    // Small cards fit, MIFARE Classic does not
    mu_check(
        nfc_read_memory_is_enough(NfcProtocolInvalid, MODERATE_FREE_HEAP, MODERATE_MAX_BLOCK));
    mu_check(
        nfc_read_memory_is_enough(NfcProtocolIso14443_4a, MODERATE_FREE_HEAP, MODERATE_MAX_BLOCK));
    mu_check(nfc_read_memory_is_enough(
        NfcProtocolMfUltralight, MODERATE_FREE_HEAP, MODERATE_MAX_BLOCK));
    mu_check(
        !nfc_read_memory_is_enough(NfcProtocolMfClassic, MODERATE_FREE_HEAP, MODERATE_MAX_BLOCK));
}

MU_TEST(nfc_read_memory_idle_heap_test) {
    for(NfcProtocol protocol = 0; protocol < NfcProtocolNum; protocol++) {
        mu_check(nfc_read_memory_is_enough(protocol, IDLE_FREE_HEAP, IDLE_MAX_BLOCK));
    }
}

MU_TEST(nfc_read_memory_boundary_test) {
    const size_t required = nfc_read_memory_required(NfcProtocolMfClassic);
    const size_t block = NFC_READ_MEMORY_MIN_HEAP_BLOCK;

    mu_check(nfc_read_memory_is_enough(NfcProtocolMfClassic, required, block));
    mu_check(!nfc_read_memory_is_enough(NfcProtocolMfClassic, required - 1, block));
    mu_check(!nfc_read_memory_is_enough(NfcProtocolMfClassic, required, block - 1));
    mu_check(!nfc_read_memory_is_enough(NfcProtocolInvalid, IDLE_FREE_HEAP, FRAGMENTED_MAX_BLOCK));
}

MU_TEST_SUITE(test_nfc_read_memory_suite) {
    MU_RUN_TEST(nfc_read_memory_required_test);
    MU_RUN_TEST(nfc_read_memory_low_heap_test);
    MU_RUN_TEST(nfc_read_memory_moderate_heap_test);
    MU_RUN_TEST(nfc_read_memory_idle_heap_test);
    MU_RUN_TEST(nfc_read_memory_boundary_test);
}

int run_minunit_test_nfc_read_memory(void) {
    MU_RUN_SUITE(test_nfc_read_memory_suite);
    return MU_EXIT_CODE;
}

TEST_API_DEFINE(run_minunit_test_nfc_read_memory)
