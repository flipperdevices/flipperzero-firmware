#include <furi.h>
#include <furi_hal.h>
#include <storage/storage.h>

#include <lib/nfc/nfc_dev.h>
#include <lib/nfc/nfc_device_data.h>
#include <lib/nfc/helpers/nfc_data_generator.h>

#include <lib/nfc/nfc.h>
#include <lib/nfc/protocols/nfca/nfca_poller.h>
#include <lib/nfc/protocols/nfca/nfca_listener.h>
#include <lib/nfc/protocols/mf_ultralight/mf_ultralight_poller.h>
#include <lib/nfc/protocols/mf_ultralight/mf_ultralight_listener.h>

#include "../minunit.h"

#define TAG "NfcTest"

#define NFC_TEST_NFC_DEV_PATH EXT_PATH("unit_tests/nfc/nfc_dev_test.nfc")

typedef struct {
    Storage* storage;
} NfcTest;

static NfcTest* nfc_test = NULL;

static void nfc_test_alloc() {
    nfc_test = malloc(sizeof(NfcTest));
    nfc_test->storage = furi_record_open(RECORD_STORAGE);
}

static void nfc_test_free() {
    furi_assert(nfc_test);

    furi_record_close(RECORD_STORAGE);
    free(nfc_test);
    nfc_test = NULL;
}

static void nfc_test_save_and_load(NfcDevData* data) {
    NfcDev* nfc_dev = nfc_dev_alloc();

    NfcDevData* nfc_dev_data_dut = malloc(sizeof(NfcDevData));

    mu_assert(nfc_dev_save(nfc_dev, data, NFC_TEST_NFC_DEV_PATH), "nfc_dev_save() failed\r\n");

    mu_assert(
        nfc_dev_load(nfc_dev, nfc_dev_data_dut, NFC_TEST_NFC_DEV_PATH),
        "nfc_dev_load() failed\r\n");

    mu_assert(
        memcmp(nfc_dev_data_dut, data, sizeof(NfcDevData)) == 0,
        "nfc_dev_data_dut != nfc_dev_data_ref\r\n");

    mu_assert(
        storage_simply_remove(nfc_test->storage, NFC_TEST_NFC_DEV_PATH),
        "storage_simply_remove() failed\r\n");

    free(nfc_dev_data_dut);
    nfc_dev_free(nfc_dev);
}

static void nfca_file_test(uint8_t uid_len) {
    NfcDevData* nfc_dev_data_ref = malloc(sizeof(NfcDevData));
    mu_assert(nfc_dev_data_ref != NULL, "malloc() failed\r\n");

    NfcaData* data = &nfc_dev_data_ref->nfca_data;

    data->uid_len = uid_len;
    furi_hal_random_fill_buf(data->uid, uid_len);
    furi_hal_random_fill_buf(data->atqa, 2);
    furi_hal_random_fill_buf(&data->sak, 1);

    nfc_test_save_and_load(nfc_dev_data_ref);

    free(nfc_dev_data_ref);
}

static void nfc_file_test_with_generator(NfcDataGeneratorType type) {
    NfcDevData* nfc_dev_data_ref = malloc(sizeof(NfcDevData));

    nfc_data_generator_fill_data(type, nfc_dev_data_ref);
    nfc_test_save_and_load(nfc_dev_data_ref);

    free(nfc_dev_data_ref);
}

MU_TEST(nfca_4b_file_test) {
    nfca_file_test(4);
}

MU_TEST(nfca_7b_file_test) {
    nfca_file_test(7);
}

MU_TEST(mf_ultralight_file_test) {
    nfc_file_test_with_generator(NfcDataGeneratorTypeMfUltralight);
}

MU_TEST(mf_ultralight_ev1_11_file_test) {
    nfc_file_test_with_generator(NfcDataGeneratorTypeMfUltralightEV1_11);
}

MU_TEST(mf_ultralight_ev1_h11_file_test) {
    nfc_file_test_with_generator(NfcDataGeneratorTypeMfUltralightEV1_H11);
}

MU_TEST(mf_ultralight_ev1_21_file_test) {
    nfc_file_test_with_generator(NfcDataGeneratorTypeMfUltralightEV1_21);
}

MU_TEST(mf_ultralight_ev1_h21_file_test) {
    nfc_file_test_with_generator(NfcDataGeneratorTypeMfUltralightEV1_H21);
}

MU_TEST(mf_ultralight_ntag_203_file_test) {
    nfc_file_test_with_generator(NfcDataGeneratorTypeNTAG203);
}

MU_TEST(mf_ultralight_ntag_213_file_test) {
    nfc_file_test_with_generator(NfcDataGeneratorTypeNTAG213);
}

MU_TEST(mf_ultralight_ntag_215_file_test) {
    nfc_file_test_with_generator(NfcDataGeneratorTypeNTAG215);
}

MU_TEST(mf_ultralight_ntag_216_file_test) {
    nfc_file_test_with_generator(NfcDataGeneratorTypeNTAG216);
}

MU_TEST(mf_ultralight_ntag_i2c_1k_file_test) {
    nfc_file_test_with_generator(NfcDataGeneratorTypeNTAGI2C1k);
}

MU_TEST(mf_ultralight_ntag_i2c_2k_file_test) {
    nfc_file_test_with_generator(NfcDataGeneratorTypeNTAGI2C2k);
}

MU_TEST(mf_ultralight_ntag_i2c_plus_1k_file_test) {
    nfc_file_test_with_generator(NfcDataGeneratorTypeNTAGI2CPlus1k);
}

MU_TEST(mf_ultralight_ntag_i2c_plus_2k_file_test) {
    nfc_file_test_with_generator(NfcDataGeneratorTypeNTAGI2CPlus2k);
}

MU_TEST(mf_classic_mini_file_test) {
    nfc_file_test_with_generator(NfcDataGeneratorTypeMfClassicMini);
}

MU_TEST(mf_classic_1k_4b_file_test) {
    nfc_file_test_with_generator(NfcDataGeneratorTypeMfClassic1k_4b);
}

MU_TEST(mf_classic_1k_7b_file_test) {
    nfc_file_test_with_generator(NfcDataGeneratorTypeMfClassic1k_7b);
}

MU_TEST(mf_classic_4k_4b_file_test) {
    nfc_file_test_with_generator(NfcDataGeneratorTypeMfClassic4k_4b);
}

MU_TEST(mf_classic_4k_7b_file_test) {
    nfc_file_test_with_generator(NfcDataGeneratorTypeMfClassic4k_7b);
}

MU_TEST(nfca_reader) {
    Nfc* poller = nfc_alloc();
    Nfc* listener = nfc_alloc();

    NfcaPoller* nfca_poller = nfca_poller_alloc(poller);
    NfcaListener* nfca_listener = nfca_listener_alloc(listener);

    NfcaData nfca_listener_data = {
        .uid_len = 7,
        .uid = {0x04, 0x51, 0x5C, 0xFA, 0x6F, 0x73, 0x81},
        .atqa = {0x44, 0x00},
        .sak = 0x00,
    };
    // nfca_listener_data.uid_len = 7;
    // furi_hal_random_fill_buf(nfca_listener_data.uid, 7);
    // furi_hal_random_fill_buf(nfca_listener_data.atqa, 2);
    // furi_hal_random_fill_buf(&nfca_listener_data.sak, 1);
    NfcaData nfca_poller_data = {};

    mu_assert(
        nfca_listener_start(nfca_listener, &nfca_listener_data, NULL, NULL) == NfcaErrorNone,
        "nfca_listener_start() failed");

    mu_assert(
        nfca_poller_read(nfca_poller, &nfca_poller_data) == NfcaErrorNone,
        "nfca_poller_read() failed");
    mu_assert(nfca_listener_stop(nfca_listener) == NfcaErrorNone, "nfca_listener_stop() failed");

    mu_assert(
        memcmp(&nfca_poller_data, &nfca_listener_data, sizeof(NfcaData)) == 0, "Data not matches");

    nfca_listener_free(nfca_listener);
    nfc_free(listener);
    nfca_poller_free(nfca_poller);
    nfc_free(poller);
}

static void mf_ultralight_reader_test(const char* path) {
    FURI_LOG_I(TAG, "Testing file: %s", path);
    NfcDev* nfc_dev = nfc_dev_alloc();
    Nfc* poller = nfc_alloc();
    Nfc* listener = nfc_alloc();

    NfcaPoller* nfca_poller = nfca_poller_alloc(poller);
    NfcaListener* nfca_listener = nfca_listener_alloc(listener);

    MfUltralightPoller* mfu_poller = mf_ultralight_poller_alloc(nfca_poller);
    MfUltralightListener* mfu_listener = mf_ultralight_listener_alloc(nfca_listener);

    MfUltralightError error = MfUltralightErrorNone;
    NfcDevData* dev_data = malloc(sizeof(NfcDevData));

    mu_assert(nfc_dev_load(nfc_dev, dev_data, path), "nfc_dev_load() failed\r\n");

    error = mf_ultralight_listener_start(mfu_listener, &dev_data->mf_ul_data, NULL, NULL);
    mu_assert(error == MfUltralightErrorNone, "mf_ultralight_listener_start() failed");

    MfUltralightData* mfu_data = malloc(sizeof(MfUltralightData));
    error = mf_ultralight_poller_read_card(mfu_poller, mfu_data);
    mu_assert(error == MfUltralightErrorNone, "mf_ultralight_poller_read_card() failed");

    error = mf_ultralight_listener_stop(mfu_listener);
    mu_assert(error == MfUltralightErrorNone, "mf_ultralight_listener_stop() failed");

    mu_assert(
        memcmp(mfu_data, &dev_data->mf_ul_data, sizeof(MfUltralightData)) == 0,
        "Data not matches");

    free(mfu_data);
    free(dev_data);
    mf_ultralight_listener_free(mfu_listener);
    mf_ultralight_poller_free(mfu_poller);
    nfca_listener_free(nfca_listener);
    nfc_free(listener);
    nfca_poller_free(nfca_poller);
    nfc_free(poller);
    nfc_dev_free(nfc_dev);
}

MU_TEST(mf_ultralight_11_reader) {
    mf_ultralight_reader_test(EXT_PATH("unit_tests/nfc/Ultralight_11.nfc"));
}

MU_TEST(mf_ultralight_21_reader) {
    mf_ultralight_reader_test(EXT_PATH("unit_tests/nfc/Ultralight_21.nfc"));
}

MU_TEST(ntag_215_reader) {
    mf_ultralight_reader_test(EXT_PATH("unit_tests/nfc/Ntag215.nfc"));
}

MU_TEST(ntag_216_reader) {
    mf_ultralight_reader_test(EXT_PATH("unit_tests/nfc/Ntag216.nfc"));
}

MU_TEST(ntag_213_locked_reader) {
    FURI_LOG_I(TAG, "Testing Ntag215 locked file");
    NfcDev* nfc_dev = nfc_dev_alloc();
    Nfc* poller = nfc_alloc();
    Nfc* listener = nfc_alloc();

    NfcaPoller* nfca_poller = nfca_poller_alloc(poller);
    NfcaListener* nfca_listener = nfca_listener_alloc(listener);

    MfUltralightPoller* mfu_poller = mf_ultralight_poller_alloc(nfca_poller);
    MfUltralightListener* mfu_listener = mf_ultralight_listener_alloc(nfca_listener);

    MfUltralightError error = MfUltralightErrorNone;
    NfcDevData* dev_data = malloc(sizeof(NfcDevData));

    mu_assert(
        nfc_dev_load(nfc_dev, dev_data, EXT_PATH("unit_tests/nfc/Ntag213_locked.nfc")),
        "nfc_dev_load() failed\r\n");

    error = mf_ultralight_listener_start(mfu_listener, &dev_data->mf_ul_data, NULL, NULL);
    mu_assert(error == MfUltralightErrorNone, "mf_ultralight_listener_start() failed");

    MfUltralightData* mfu_data = malloc(sizeof(MfUltralightData));
    error = mf_ultralight_poller_read_card(mfu_poller, mfu_data);
    mu_assert(error == MfUltralightErrorNone, "mf_ultralight_poller_read_card() failed");

    error = mf_ultralight_listener_stop(mfu_listener);
    mu_assert(error == MfUltralightErrorNone, "mf_ultralight_listener_stop() failed");

    MfUltralightConfigPages* config = NULL;
    mu_assert(
        mf_ultralight_get_config_page(&dev_data->mf_ul_data, &config),
        "mf_ultralight_get_config_page() failed");
    uint16_t pages_locked = config->auth0;

    mu_assert(mfu_data->pages_read == pages_locked, "Unexpected pages read");

    free(mfu_data);
    free(dev_data);
    mf_ultralight_listener_free(mfu_listener);
    mf_ultralight_poller_free(mfu_poller);
    nfca_listener_free(nfca_listener);
    nfc_free(listener);
    nfca_poller_free(nfca_poller);
    nfc_free(poller);
    nfc_dev_free(nfc_dev);
}

static void mf_ultralight_write() {
    NfcDev* nfc_dev = nfc_dev_alloc();
    Nfc* poller = nfc_alloc();
    Nfc* listener = nfc_alloc();

    NfcaPoller* nfca_poller = nfca_poller_alloc(poller);
    NfcaListener* nfca_listener = nfca_listener_alloc(listener);

    MfUltralightPoller* mfu_poller = mf_ultralight_poller_alloc(nfca_poller);
    MfUltralightListener* mfu_listener = mf_ultralight_listener_alloc(nfca_listener);

    MfUltralightError error = MfUltralightErrorNone;
    NfcDevData* dev_data = malloc(sizeof(NfcDevData));
    nfc_data_generator_fill_data(NfcDataGeneratorTypeMfUltralightEV1_21, dev_data);

    error = mf_ultralight_listener_start(mfu_listener, &dev_data->mf_ul_data, NULL, NULL);
    mu_assert(error == MfUltralightErrorNone, "mf_ultralight_listener_start() failed");

    MfUltralightData* mfu_data = malloc(sizeof(MfUltralightData));

    // Initial read
    error = mf_ultralight_poller_read_card(mfu_poller, mfu_data);
    mu_assert(error == MfUltralightErrorNone, "mf_ultralight_poller_read_card() failed");

    mu_assert(
        memcmp(mfu_data, &dev_data->mf_ul_data, sizeof(MfUltralightData)) == 0,
        "Data not matches");

    // Write random data
    for(size_t i = 5; i < 15; i++) {
        MfUltralightPage page = {};
        FURI_LOG_D(TAG, "Writing page %d", i);
        furi_hal_random_fill_buf(page.data, sizeof(MfUltralightPage));
        mfu_data->page[i] = page;
        error = mf_ultralight_poller_write_page(mfu_poller, i, &page);
        mu_assert(error == MfUltralightErrorNone, "mf_ultralight_poller_read_card() failed");
    }

    // Verification read
    error = mf_ultralight_poller_read_card(mfu_poller, mfu_data);
    mu_assert(error == MfUltralightErrorNone, "mf_ultralight_poller_read_card() failed");

    mf_ultralight_listener_get_data(mfu_listener, &dev_data->mf_ul_data);
    error = mf_ultralight_listener_stop(mfu_listener);

    mu_assert(error == MfUltralightErrorNone, "mf_ultralight_listener_stop() failed");

    mu_assert(
        memcmp(mfu_data, &dev_data->mf_ul_data, sizeof(MfUltralightData)) == 0,
        "Data not matches");

    free(mfu_data);
    free(dev_data);
    mf_ultralight_listener_free(mfu_listener);
    mf_ultralight_poller_free(mfu_poller);
    nfca_listener_free(nfca_listener);
    nfc_free(listener);
    nfca_poller_free(nfca_poller);
    nfc_free(poller);
    nfc_dev_free(nfc_dev);
}

MU_TEST_SUITE(nfc) {
    nfc_test_alloc();

    MU_RUN_TEST(nfca_reader);
    MU_RUN_TEST(mf_ultralight_11_reader);
    MU_RUN_TEST(mf_ultralight_21_reader);
    MU_RUN_TEST(ntag_215_reader);
    MU_RUN_TEST(ntag_216_reader);
    MU_RUN_TEST(ntag_213_locked_reader);

    MU_RUN_TEST(mf_ultralight_write);

    MU_RUN_TEST(nfca_4b_file_test);
    MU_RUN_TEST(nfca_7b_file_test);

    MU_RUN_TEST(mf_ultralight_file_test);
    MU_RUN_TEST(mf_ultralight_ev1_11_file_test);
    MU_RUN_TEST(mf_ultralight_ev1_h11_file_test);
    MU_RUN_TEST(mf_ultralight_ev1_21_file_test);
    MU_RUN_TEST(mf_ultralight_ev1_h21_file_test);
    MU_RUN_TEST(mf_ultralight_ntag_203_file_test);
    MU_RUN_TEST(mf_ultralight_ntag_213_file_test);
    MU_RUN_TEST(mf_ultralight_ntag_215_file_test);
    MU_RUN_TEST(mf_ultralight_ntag_216_file_test);
    MU_RUN_TEST(mf_ultralight_ntag_i2c_1k_file_test);
    MU_RUN_TEST(mf_ultralight_ntag_i2c_2k_file_test);
    MU_RUN_TEST(mf_ultralight_ntag_i2c_plus_1k_file_test);
    MU_RUN_TEST(mf_ultralight_ntag_i2c_plus_2k_file_test);

    MU_RUN_TEST(mf_classic_mini_file_test);
    MU_RUN_TEST(mf_classic_1k_4b_file_test);
    MU_RUN_TEST(mf_classic_1k_7b_file_test);
    MU_RUN_TEST(mf_classic_4k_4b_file_test);
    MU_RUN_TEST(mf_classic_4k_7b_file_test);

    nfc_test_free();
}

int run_minunit_test_nfc() {
    MU_RUN_SUITE(nfc);
    return MU_EXIT_CODE;
}
