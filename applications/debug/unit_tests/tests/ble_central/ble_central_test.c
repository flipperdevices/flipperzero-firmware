#include <furi.h>
#include "../test.h" // IWYU pragma: keep

#include <furi_ble/central.h>

/* Advertising-data parsing is pure and needs no radio, so it is worth testing
 * even though the rest of the central API requires a peer. The bounds
 * arithmetic in particular is easy to get wrong by one and hard to notice:
 * a malformed or truncated advertisement is not rare on air. */

static BleCentralAdvReport make_report(const uint8_t* data, uint8_t len) {
    BleCentralAdvReport report = {0};
    report.data_len = len;
    if(len) memcpy(report.data, data, len);
    return report;
}

MU_TEST(ble_central_test_adv_find_complete_name) {
    /* 0x0C 0x09 "Tonal 26613" — a real scan response. */
    const uint8_t adv[] = {0x0C, 0x09, 'T', 'o', 'n', 'a', 'l', ' ', '2', '6', '6', '1', '3'};
    BleCentralAdvReport report = make_report(adv, sizeof(adv));

    const uint8_t* value = NULL;
    uint8_t len = 0;
    mu_assert(ble_central_adv_find(&report, 0x09, &value, &len), "name not found");
    mu_assert_int_eq(11, len);
    mu_assert(memcmp(value, "Tonal 26613", 11) == 0, "wrong name bytes");
}

MU_TEST(ble_central_test_adv_find_walks_past_earlier_records) {
    /* Flags, then manufacturer data, then the name: the target is last. */
    const uint8_t adv[] = {
        0x02, 0x01, 0x1A, 0x05, 0xFF, 0x4C, 0x00, 0x09, 0x08, 0x03, 0x09, 'H', 'i'};
    BleCentralAdvReport report = make_report(adv, sizeof(adv));

    const uint8_t* value = NULL;
    uint8_t len = 0;
    mu_assert(ble_central_adv_find(&report, 0x09, &value, &len), "name not found");
    mu_assert_int_eq(2, len);
    mu_assert(memcmp(value, "Hi", 2) == 0, "wrong name bytes");

    /* And the earlier records are still reachable by their own type. */
    mu_assert(ble_central_adv_find(&report, 0x01, &value, &len), "flags not found");
    mu_assert_int_eq(1, len);
    mu_assert_int_eq(0x1A, value[0]);
}

MU_TEST(ble_central_test_adv_find_absent_type) {
    const uint8_t adv[] = {0x02, 0x01, 0x06};
    BleCentralAdvReport report = make_report(adv, sizeof(adv));

    mu_assert(!ble_central_adv_find(&report, 0x09, NULL, NULL), "found a type that is absent");
}

MU_TEST(ble_central_test_adv_find_empty_payload) {
    BleCentralAdvReport report = make_report(NULL, 0);
    mu_assert(!ble_central_adv_find(&report, 0x09, NULL, NULL), "found a type in empty data");
}

MU_TEST(ble_central_test_adv_find_rejects_overrunning_record) {
    /* Length claims 20 bytes but only 3 are present. Accepting this reads past
     * the end of the advertisement; an earlier version of this walk did. */
    const uint8_t adv[] = {0x14, 0x09, 'A'};
    BleCentralAdvReport report = make_report(adv, sizeof(adv));

    mu_assert(!ble_central_adv_find(&report, 0x09, NULL, NULL), "accepted a record that overruns");
}

MU_TEST(ble_central_test_adv_find_exact_fit) {
    /* The record ends exactly at the last byte, which must still be accepted:
     * off-by-one guards that are too strict drop valid advertisements. */
    const uint8_t adv[] = {0x03, 0x09, 'A', 'B'};
    BleCentralAdvReport report = make_report(adv, sizeof(adv));

    const uint8_t* value = NULL;
    uint8_t len = 0;
    mu_assert(ble_central_adv_find(&report, 0x09, &value, &len), "exact-fit record rejected");
    mu_assert_int_eq(2, len);
    mu_assert(memcmp(value, "AB", 2) == 0, "wrong bytes");
}

MU_TEST(ble_central_test_adv_find_zero_length_terminates) {
    /* A zero length is the padding that ends an advertisement. It must stop the
     * walk rather than loop forever on a record that never advances. */
    const uint8_t adv[] = {0x02, 0x01, 0x06, 0x00, 0x00, 0x00};
    BleCentralAdvReport report = make_report(adv, sizeof(adv));

    mu_assert(!ble_central_adv_find(&report, 0x09, NULL, NULL), "did not stop at padding");
    mu_assert(ble_central_adv_find(&report, 0x01, NULL, NULL), "lost a record before padding");
}

MU_TEST(ble_central_test_adv_find_null_out_params) {
    const uint8_t adv[] = {0x03, 0x09, 'A', 'B'};
    BleCentralAdvReport report = make_report(adv, sizeof(adv));

    /* Callers that only need presence pass NULL for both. */
    mu_assert(ble_central_adv_find(&report, 0x09, NULL, NULL), "presence check failed");
}

MU_TEST(ble_central_test_adv_find_truncated_after_length) {
    /* The length byte promises a value byte that was never transmitted. */
    const uint8_t adv[] = {0x02, 0x01};
    BleCentralAdvReport report = make_report(adv, sizeof(adv));

    mu_assert(!ble_central_adv_find(&report, 0x01, NULL, NULL), "accepted a truncated record");
}

MU_TEST(ble_central_test_adv_find_dangling_length_byte) {
    /* A complete record followed by a lone length byte with no type after it.
     * The walk must stop rather than read data[i + 1] past the end. */
    const uint8_t adv[] = {0x02, 0x01, 0x06, 0x03};
    BleCentralAdvReport report = make_report(adv, sizeof(adv));

    mu_assert(ble_central_adv_find(&report, 0x01, NULL, NULL), "lost the complete record");
    mu_assert(!ble_central_adv_find(&report, 0x09, NULL, NULL), "read past the dangling length");
}

MU_TEST(ble_central_test_adv_find_ignores_type_byte_inside_a_value) {
    /* The manufacturer-data payload contains 0x09, the byte a naive scan would
     * mistake for a Complete Local Name record. Walking by length skips it. */
    const uint8_t adv[] = {0x05, 0xFF, 0x4C, 0x00, 0x09, 0x09};
    BleCentralAdvReport report = make_report(adv, sizeof(adv));

    mu_assert(
        !ble_central_adv_find(&report, 0x09, NULL, NULL), "matched a byte inside another value");
    mu_assert(ble_central_adv_find(&report, 0xFF, NULL, NULL), "lost the manufacturer record");
}

MU_TEST(ble_central_test_adv_find_first_match_wins) {
    const uint8_t adv[] = {0x03, 0x09, 'A', 'B', 0x03, 0x09, 'C', 'D'};
    BleCentralAdvReport report = make_report(adv, sizeof(adv));

    const uint8_t* value = NULL;
    uint8_t len = 0;
    mu_assert(ble_central_adv_find(&report, 0x09, &value, &len), "name not found");
    mu_assert(memcmp(value, "AB", 2) == 0, "did not return the first match");
}

MU_TEST(ble_central_test_adv_find_zero_length_value) {
    /* A type with no value is well-formed: length 1 covers the type byte alone.
     * It must report presence with a length of zero, not be treated as padding. */
    const uint8_t adv[] = {0x01, 0x09};
    BleCentralAdvReport report = make_report(adv, sizeof(adv));

    const uint8_t* value = NULL;
    uint8_t len = 0xFF;
    mu_assert(ble_central_adv_find(&report, 0x09, &value, &len), "zero-length record rejected");
    mu_assert_int_eq(0, len);
}

MU_TEST(ble_central_test_adv_find_full_payload) {
    /* 31 bytes is the maximum a legacy advertisement can carry, so the last
     * record ends exactly at the buffer limit. */
    uint8_t adv[BLE_CENTRAL_ADV_DATA_MAX];
    memset(adv, 0, sizeof(adv));
    adv[0] = 0x02;
    adv[1] = 0x01;
    adv[2] = 0x06;
    adv[3] = 27; /* type + 26 value bytes, ending on byte 30 */
    adv[4] = 0x09;
    memset(&adv[5], 'x', 26);
    BleCentralAdvReport report = make_report(adv, sizeof(adv));

    const uint8_t* value = NULL;
    uint8_t len = 0;
    mu_assert(ble_central_adv_find(&report, 0x09, &value, &len), "full-payload record rejected");
    mu_assert_int_eq(26, len);
    mu_assert_int_eq('x', value[25]);
}

MU_TEST(ble_central_test_adv_find_all_record_lengths) {
    /* Exhaust the one-byte record length against every possible legacy payload
     * size. This guards both integer wraparound and accepting a record whose
     * claimed value extends beyond the received bytes. */
    uint8_t adv[BLE_CENTRAL_ADV_DATA_MAX] = {0};
    for(uint8_t payload_len = 1; payload_len <= BLE_CENTRAL_ADV_DATA_MAX; payload_len++) {
        for(uint16_t record_len = 0; record_len <= UINT8_MAX; record_len++) {
            memset(adv, 0xA5, sizeof(adv));
            adv[0] = record_len;
            if(payload_len > 1) adv[1] = 0x09;

            BleCentralAdvReport report = make_report(adv, payload_len);
            bool expected = record_len >= 1 && record_len + 1 <= payload_len;
            mu_assert(
                ble_central_adv_find(&report, 0x09, NULL, NULL) == expected,
                "record-length bounds mismatch");
        }
    }
}

MU_TEST_SUITE(test_ble_central) {
    MU_RUN_TEST(ble_central_test_adv_find_complete_name);
    MU_RUN_TEST(ble_central_test_adv_find_walks_past_earlier_records);
    MU_RUN_TEST(ble_central_test_adv_find_absent_type);
    MU_RUN_TEST(ble_central_test_adv_find_empty_payload);
    MU_RUN_TEST(ble_central_test_adv_find_rejects_overrunning_record);
    MU_RUN_TEST(ble_central_test_adv_find_exact_fit);
    MU_RUN_TEST(ble_central_test_adv_find_zero_length_terminates);
    MU_RUN_TEST(ble_central_test_adv_find_null_out_params);
    MU_RUN_TEST(ble_central_test_adv_find_truncated_after_length);
    MU_RUN_TEST(ble_central_test_adv_find_dangling_length_byte);
    MU_RUN_TEST(ble_central_test_adv_find_ignores_type_byte_inside_a_value);
    MU_RUN_TEST(ble_central_test_adv_find_first_match_wins);
    MU_RUN_TEST(ble_central_test_adv_find_zero_length_value);
    MU_RUN_TEST(ble_central_test_adv_find_full_payload);
    MU_RUN_TEST(ble_central_test_adv_find_all_record_lengths);
}

int run_minunit_test_ble_central(void) {
    MU_RUN_SUITE(test_ble_central);
    return MU_EXIT_CODE;
}

TEST_API_DEFINE(run_minunit_test_ble_central)
