#include <furi.h>
#include <furi_hal.h>
#include <storage/storage.h>

#include <nfc/nfc_device.h>
#include <nfc/helpers/nfc_data_generator.h>
#include <nfc/nfc_poller.h>
#include <nfc/nfc_listener.h>
#include <nfc/protocols/iso14443_3a/iso14443_3a.h>
#include <nfc/protocols/iso14443_3a/iso14443_3a_poller.h>
#include <nfc/protocols/iso14443_3a/iso14443_3a_poller_sync.h>
#include <nfc/protocols/mf_ultralight/mf_ultralight.h>
#include <nfc/protocols/mf_ultralight/mf_ultralight_poller_sync.h>
#include <nfc/protocols/mf_classic/mf_classic_poller_sync.h>
#include <nfc/protocols/felica/felica.h>
#include <nfc/protocols/felica/felica_poller_sync.h>
#include <nfc/protocols/mf_classic/mf_classic_poller.h>
#include <nfc/protocols/iso15693_3/iso15693_3_poller.h>
#include <nfc/protocols/slix/slix.h>
#include <nfc/protocols/slix/slix_i.h>
#include <nfc/protocols/slix/slix_poller.h>
#include <nfc/protocols/slix/slix_poller_i.h>
#include <nfc/protocols/legic_prime/legic_prime.h>
#include <nfc/protocols/legic_prime/legic_prime_crc.h>
#include <nfc/protocols/legic_prime/legic_prime_listener_i.h>
#include <nfc/protocols/legic_prime/legic_prime_prng.h>
#include <nfc/protocols/legic_prime/legic_prime_poller_i.h>

#include <nfc/nfc_poller.h>

#include <toolbox/keys_dict.h>
#include <nfc/nfc.h>

#include "../test.h" // IWYU pragma: keep

#define TAG "NfcTest"

#define NFC_TEST_NFC_DEV_PATH                  EXT_PATH("unit_tests/nfc/nfc_device_test.nfc")
#define NFC_APP_MF_CLASSIC_DICT_UNIT_TEST_PATH EXT_PATH("unit_tests/mf_dict.nfc")

#define NFC_TEST_FLAG_WORKER_DONE (1)

typedef enum {
    NfcTestMfClassicSendFrameTestStateAuth,
    NfcTestMfClassicSendFrameTestStateReadBlock,

    NfcTestMfClassicSendFrameTestStateFail,
    NfcTestMfClassicSendFrameTestStateSuccess,
} NfcTestMfClassicSendFrameTestState;

typedef struct {
    NfcTestMfClassicSendFrameTestState state;
    BitBuffer* tx_buf;
    BitBuffer* rx_buf;
    FuriThreadId thread_id;
} NfcTestMfClassicSendFrameTest;

typedef enum {
    NfcTestSlixPollerSetPasswordStateGetRandomNumber,
    NfcTestSlixPollerSetPasswordStateSetPassword,
} NfcTestSlixPollerSetPasswordState;

typedef struct {
    FuriThreadId thread_id;
    NfcTestSlixPollerSetPasswordState state;
    SlixRandomNumber random_number;
    SlixPassword password;
    SlixError error;
} NfcTestSlixPollerSetPasswordContext;

typedef struct {
    Storage* storage;
} NfcTest;

static NfcTest* nfc_test = NULL;

static void nfc_test_alloc(void) {
    nfc_test = malloc(sizeof(NfcTest));
    nfc_test->storage = furi_record_open(RECORD_STORAGE);
}

static void nfc_test_free(void) {
    furi_check(nfc_test);

    furi_record_close(RECORD_STORAGE);
    free(nfc_test);
    nfc_test = NULL;
}

static void nfc_test_save_and_load(NfcDevice* nfc_device_ref) {
    NfcDevice* nfc_device_dut = nfc_device_alloc();

    mu_assert(
        nfc_device_save(nfc_device_ref, NFC_TEST_NFC_DEV_PATH), "nfc_device_save() failed\r\n");

    mu_assert(
        nfc_device_load(nfc_device_dut, NFC_TEST_NFC_DEV_PATH), "nfc_device_load() failed\r\n");

    mu_assert(
        nfc_device_is_equal(nfc_device_ref, nfc_device_dut),
        "nfc_device_data_dut != nfc_device_data_ref\r\n");

    mu_assert(
        storage_simply_remove(nfc_test->storage, NFC_TEST_NFC_DEV_PATH),
        "storage_simply_remove() failed\r\n");

    nfc_device_free(nfc_device_dut);
}

static void iso14443_3a_file_test(uint8_t uid_len) {
    NfcDevice* nfc_device = nfc_device_alloc();

    Iso14443_3aData* data = iso14443_3a_alloc();
    data->uid_len = uid_len;
    furi_hal_random_fill_buf(data->uid, uid_len);
    furi_hal_random_fill_buf(data->atqa, sizeof(data->atqa));
    furi_hal_random_fill_buf(&data->sak, 1);

    nfc_device_set_data(nfc_device, NfcProtocolIso14443_3a, data);
    nfc_test_save_and_load(nfc_device);

    iso14443_3a_free(data);
    nfc_device_free(nfc_device);
}

static void nfc_file_test_with_generator(NfcDataGeneratorType type) {
    NfcDevice* nfc_device_ref = nfc_device_alloc();

    nfc_data_generator_fill_data(type, nfc_device_ref);
    nfc_test_save_and_load(nfc_device_ref);

    nfc_device_free(nfc_device_ref);
}

MU_TEST(iso14443_3a_4b_file_test) {
    iso14443_3a_file_test(4);
}

MU_TEST(iso14443_3a_7b_file_test) {
    iso14443_3a_file_test(7);
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

MU_TEST(iso14443_3a_reader) {
    Nfc* poller = nfc_alloc();
    Nfc* listener = nfc_alloc();

    Iso14443_3aData iso14443_3a_listener_data = {
        .uid_len = 7,
        .uid = {0x04, 0x51, 0x5C, 0xFA, 0x6F, 0x73, 0x81},
        .atqa = {0x44, 0x00},
        .sak = 0x00,
    };
    NfcListener* iso3_listener =
        nfc_listener_alloc(listener, NfcProtocolIso14443_3a, &iso14443_3a_listener_data);
    nfc_listener_start(iso3_listener, NULL, NULL);

    Iso14443_3aData iso14443_3a_poller_data = {};
    mu_assert(
        iso14443_3a_poller_sync_read(poller, &iso14443_3a_poller_data) == Iso14443_3aErrorNone,
        "iso14443_3a_poller_sync_read() failed");

    nfc_listener_stop(iso3_listener);
    mu_assert(
        iso14443_3a_is_equal(&iso14443_3a_poller_data, &iso14443_3a_listener_data),
        "Data not matches");

    nfc_listener_free(iso3_listener);
    nfc_free(listener);
    nfc_free(poller);
}

static void mf_ultralight_reader_test(const char* path) {
    FURI_LOG_I(TAG, "Testing file: %s", path);
    Nfc* poller = nfc_alloc();
    Nfc* listener = nfc_alloc();

    NfcDevice* nfc_device = nfc_device_alloc();
    mu_assert(nfc_device_load(nfc_device, path), "nfc_device_load() failed\r\n");

    MfUltralightData* data =
        (MfUltralightData*)nfc_device_get_data(nfc_device, NfcProtocolMfUltralight);

    uint32_t features = mf_ultralight_get_feature_support_set(data->type);
    bool pwd_supported =
        mf_ultralight_support_feature(features, MfUltralightFeatureSupportPasswordAuth);
    uint8_t pwd_num = mf_ultralight_get_pwd_page_num(data->type);
    const uint8_t zero_pwd[4] = {0, 0, 0, 0};

    if(pwd_supported && !memcmp(data->page[pwd_num].data, zero_pwd, sizeof(zero_pwd))) {
        data->pages_read -= 2;
    }

    NfcListener* mfu_listener = nfc_listener_alloc(listener, NfcProtocolMfUltralight, data);

    nfc_listener_start(mfu_listener, NULL, NULL);

    MfUltralightData* mfu_data = mf_ultralight_alloc();
    MfUltralightError error = mf_ultralight_poller_sync_read_card(poller, mfu_data, NULL);
    mu_assert(error == MfUltralightErrorNone, "mf_ultralight_poller_sync_read_card() failed");

    nfc_listener_stop(mfu_listener);
    nfc_listener_free(mfu_listener);

    MfUltralightData* mfu_other_data =
        (MfUltralightData*)nfc_device_get_data(nfc_device, NfcProtocolMfUltralight);
    mu_assert(mf_ultralight_is_equal(mfu_data, mfu_other_data), "Data mismatch");

    mf_ultralight_free(mfu_data);
    nfc_device_free(nfc_device);
    nfc_free(listener);
    nfc_free(poller);
}

MU_TEST(mf_ultralight_11_reader) {
    mf_ultralight_reader_test(EXT_PATH("unit_tests/nfc/Ultralight_11.nfc"));
}

MU_TEST(mf_ultralight_21_reader) {
    mf_ultralight_reader_test(EXT_PATH("unit_tests/nfc/Ultralight_21.nfc"));
}

MU_TEST(mf_ultralight_c_reader) {
    mf_ultralight_reader_test(EXT_PATH("unit_tests/nfc/Ultralight_C.nfc"));
}

MU_TEST(ntag_215_reader) {
    mf_ultralight_reader_test(EXT_PATH("unit_tests/nfc/Ntag215.nfc"));
}

MU_TEST(ntag_216_reader) {
    mf_ultralight_reader_test(EXT_PATH("unit_tests/nfc/Ntag216.nfc"));
}

MU_TEST(ntag_213_locked_reader) {
    FURI_LOG_I(TAG, "Testing Ntag215 locked file");
    Nfc* poller = nfc_alloc();
    Nfc* listener = nfc_alloc();

    NfcDeviceData* nfc_device = nfc_device_alloc();
    mu_assert(
        nfc_device_load(nfc_device, EXT_PATH("unit_tests/nfc/Ntag213_locked.nfc")),
        "nfc_device_load() failed\r\n");

    NfcListener* mfu_listener = nfc_listener_alloc(
        listener,
        NfcProtocolMfUltralight,
        nfc_device_get_data(nfc_device, NfcProtocolMfUltralight));
    nfc_listener_start(mfu_listener, NULL, NULL);

    MfUltralightData* mfu_data = mf_ultralight_alloc();
    MfUltralightError error = mf_ultralight_poller_sync_read_card(poller, mfu_data, NULL);
    mu_assert(error == MfUltralightErrorNone, "mf_ultralight_poller_sync_read_card() failed");

    nfc_listener_stop(mfu_listener);
    nfc_listener_free(mfu_listener);

    MfUltralightConfigPages* config = NULL;
    const MfUltralightData* mfu_ref_data =
        nfc_device_get_data(nfc_device, NfcProtocolMfUltralight);
    mu_assert(
        mf_ultralight_get_config_page(mfu_ref_data, &config),
        "mf_ultralight_get_config_page() failed");
    uint16_t pages_locked = config->auth0;

    mu_assert(mfu_data->pages_read == pages_locked, "Unexpected pages read");

    mf_ultralight_free(mfu_data);
    nfc_device_free(nfc_device);
    nfc_free(listener);
    nfc_free(poller);
}

static void mf_ultralight_write(void) {
    Nfc* poller = nfc_alloc();
    Nfc* listener = nfc_alloc();

    NfcDevice* nfc_device = nfc_device_alloc();
    nfc_data_generator_fill_data(NfcDataGeneratorTypeMfUltralightEV1_21, nfc_device);

    NfcListener* mfu_listener = nfc_listener_alloc(
        listener,
        NfcProtocolMfUltralight,
        nfc_device_get_data(nfc_device, NfcProtocolMfUltralight));
    nfc_listener_start(mfu_listener, NULL, NULL);

    MfUltralightData* mfu_data = mf_ultralight_alloc();

    // Initial read
    MfUltralightError error = mf_ultralight_poller_sync_read_card(poller, mfu_data, NULL);
    mu_assert(error == MfUltralightErrorNone, "mf_ultralight_poller_sync_read_card() failed");

    mu_assert(
        mf_ultralight_is_equal(mfu_data, nfc_device_get_data(nfc_device, NfcProtocolMfUltralight)),
        "Data not matches");

    // Write random data
    for(size_t i = 5; i < 15; i++) {
        MfUltralightPage page = {};
        FURI_LOG_D(TAG, "Writing page %d", i);
        furi_hal_random_fill_buf(page.data, sizeof(MfUltralightPage));
        mfu_data->page[i] = page;
        error = mf_ultralight_poller_sync_write_page(poller, i, &page);
        mu_assert(error == MfUltralightErrorNone, "mf_ultralight_poller_sync_write_page() failed");
    }

    // Verification read
    error = mf_ultralight_poller_sync_read_card(poller, mfu_data, NULL);
    mu_assert(error == MfUltralightErrorNone, "mf_ultralight_poller_sync_read_card() failed");

    nfc_listener_stop(mfu_listener);
    const MfUltralightData* mfu_listener_data =
        nfc_listener_get_data(mfu_listener, NfcProtocolMfUltralight);

    mu_assert(mf_ultralight_is_equal(mfu_data, mfu_listener_data), "Data not matches");

    nfc_listener_free(mfu_listener);
    mf_ultralight_free(mfu_data);
    nfc_device_free(nfc_device);
    nfc_free(listener);
    nfc_free(poller);
}

static void mf_classic_reader(void) {
    Nfc* poller = nfc_alloc();
    Nfc* listener = nfc_alloc();

    NfcDevice* nfc_device = nfc_device_alloc();
    nfc_data_generator_fill_data(NfcDataGeneratorTypeMfClassic4k_7b, nfc_device);
    NfcListener* mfc_listener = nfc_listener_alloc(
        listener, NfcProtocolMfClassic, nfc_device_get_data(nfc_device, NfcProtocolMfClassic));
    nfc_listener_start(mfc_listener, NULL, NULL);

    MfClassicBlock block = {};
    MfClassicKey key = {.data = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};

    mf_classic_poller_sync_read_block(poller, 0, &key, MfClassicKeyTypeA, &block);

    nfc_listener_stop(mfc_listener);
    nfc_listener_free(mfc_listener);

    const MfClassicData* mfc_data = nfc_device_get_data(nfc_device, NfcProtocolMfClassic);
    mu_assert(memcmp(&mfc_data->block[0], &block, sizeof(MfClassicBlock)) == 0, "Data mismatch");

    nfc_device_free(nfc_device);
    nfc_free(listener);
    nfc_free(poller);
}

static void mf_classic_write(void) {
    Nfc* poller = nfc_alloc();
    Nfc* listener = nfc_alloc();

    NfcDevice* nfc_device = nfc_device_alloc();
    nfc_data_generator_fill_data(NfcDataGeneratorTypeMfClassic4k_7b, nfc_device);
    NfcListener* mfc_listener = nfc_listener_alloc(
        listener, NfcProtocolMfClassic, nfc_device_get_data(nfc_device, NfcProtocolMfClassic));
    nfc_listener_start(mfc_listener, NULL, NULL);

    MfClassicBlock block_write = {};
    MfClassicBlock block_read = {};
    furi_hal_random_fill_buf(block_write.data, sizeof(MfClassicBlock));
    MfClassicKey key = {.data = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};

    mf_classic_poller_sync_write_block(poller, 1, &key, MfClassicKeyTypeA, &block_write);
    mf_classic_poller_sync_read_block(poller, 1, &key, MfClassicKeyTypeA, &block_read);

    nfc_listener_stop(mfc_listener);
    nfc_listener_free(mfc_listener);

    mu_assert(memcmp(&block_read, &block_write, sizeof(MfClassicBlock)) == 0, "Data mismatch");

    nfc_device_free(nfc_device);
    nfc_free(listener);
    nfc_free(poller);
}

static void mf_classic_value_block(void) {
    Nfc* poller = nfc_alloc();
    Nfc* listener = nfc_alloc();

    NfcDevice* nfc_device = nfc_device_alloc();
    nfc_data_generator_fill_data(NfcDataGeneratorTypeMfClassic4k_7b, nfc_device);
    NfcListener* mfc_listener = nfc_listener_alloc(
        listener, NfcProtocolMfClassic, nfc_device_get_data(nfc_device, NfcProtocolMfClassic));
    nfc_listener_start(mfc_listener, NULL, NULL);

    MfClassicKey key = {.data = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};

    int32_t value = 228;
    MfClassicBlock block_write = {};
    mf_classic_value_to_block(value, 1, &block_write);

    MfClassicError error = MfClassicErrorNone;
    error = mf_classic_poller_sync_write_block(poller, 1, &key, MfClassicKeyTypeA, &block_write);
    mu_assert(error == MfClassicErrorNone, "Write failed");

    int32_t data = 200;
    int32_t new_value = 0;
    error =
        mf_classic_poller_sync_change_value(poller, 1, &key, MfClassicKeyTypeA, data, &new_value);
    mu_assert(error == MfClassicErrorNone, "Value increment failed");
    mu_assert(new_value == value + data, "Value not match");

    error =
        mf_classic_poller_sync_change_value(poller, 1, &key, MfClassicKeyTypeA, -data, &new_value);
    mu_assert(error == MfClassicErrorNone, "Value decrement failed");
    mu_assert(new_value == value, "Value not match");

    nfc_listener_stop(mfc_listener);
    nfc_listener_free(mfc_listener);
    nfc_device_free(nfc_device);
    nfc_free(listener);
    nfc_free(poller);
}

NfcCommand mf_classic_poller_send_frame_callback(NfcGenericEventEx event, void* context) {
    furi_check(event.poller);
    furi_check(event.parent_event_data);
    furi_check(context);

    NfcCommand command = NfcCommandContinue;
    MfClassicPoller* instance = event.poller;
    NfcTestMfClassicSendFrameTest* frame_test = context;
    Iso14443_3aPollerEvent* iso3_event = event.parent_event_data;

    MfClassicError error = MfClassicErrorNone;
    if(iso3_event->type == Iso14443_3aPollerEventTypeReady) {
        if(frame_test->state == NfcTestMfClassicSendFrameTestStateAuth) {
            MfClassicKey key = {
                .data = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
            };
            error = mf_classic_poller_auth(instance, 0, &key, MfClassicKeyTypeA, NULL, false);
            frame_test->state = (error == MfClassicErrorNone) ?
                                    NfcTestMfClassicSendFrameTestStateReadBlock :
                                    NfcTestMfClassicSendFrameTestStateFail;
        } else if(frame_test->state == NfcTestMfClassicSendFrameTestStateReadBlock) {
            do {
                const uint8_t read_block_cmd[] = {
                    0x30,
                    0x01,
                    0x8b,
                    0xb9,
                };
                bit_buffer_copy_bytes(frame_test->tx_buf, read_block_cmd, sizeof(read_block_cmd));

                error = mf_classic_poller_send_encrypted_frame(
                    instance, frame_test->tx_buf, frame_test->rx_buf, 200000);
                if(error != MfClassicErrorNone) break;
                if(bit_buffer_get_size_bytes(frame_test->rx_buf) != 18) {
                    error = MfClassicErrorProtocol;
                    break;
                }

                const uint8_t* rx_data = bit_buffer_get_data(frame_test->rx_buf);
                const uint8_t rx_data_ref[16] = {0};
                if(memcmp(rx_data, rx_data_ref, sizeof(rx_data_ref)) != 0) {
                    error = MfClassicErrorProtocol;
                    break;
                }
            } while(false);

            frame_test->state = (error == MfClassicErrorNone) ?
                                    NfcTestMfClassicSendFrameTestStateSuccess :
                                    NfcTestMfClassicSendFrameTestStateFail;
        } else if(frame_test->state == NfcTestMfClassicSendFrameTestStateSuccess) {
            command = NfcCommandStop;
        } else if(frame_test->state == NfcTestMfClassicSendFrameTestStateFail) {
            command = NfcCommandStop;
        }
    } else {
        frame_test->state = NfcTestMfClassicSendFrameTestStateFail;
        command = NfcCommandStop;
    }

    if(command == NfcCommandStop) {
        furi_thread_flags_set(frame_test->thread_id, NFC_TEST_FLAG_WORKER_DONE);
    }

    return command;
}

MU_TEST(mf_classic_send_frame_test) {
    Nfc* poller = nfc_alloc();
    Nfc* listener = nfc_alloc();

    NfcDevice* nfc_device = nfc_device_alloc();
    nfc_data_generator_fill_data(NfcDataGeneratorTypeMfClassic4k_7b, nfc_device);
    NfcListener* mfc_listener = nfc_listener_alloc(
        listener, NfcProtocolMfClassic, nfc_device_get_data(nfc_device, NfcProtocolMfClassic));
    nfc_listener_start(mfc_listener, NULL, NULL);

    NfcPoller* mfc_poller = nfc_poller_alloc(poller, NfcProtocolMfClassic);
    NfcTestMfClassicSendFrameTest context = {
        .state = NfcTestMfClassicSendFrameTestStateAuth,
        .thread_id = furi_thread_get_current_id(),
        .tx_buf = bit_buffer_alloc(32),
        .rx_buf = bit_buffer_alloc(32),
    };
    nfc_poller_start_ex(mfc_poller, mf_classic_poller_send_frame_callback, &context);

    uint32_t flag =
        furi_thread_flags_wait(NFC_TEST_FLAG_WORKER_DONE, FuriFlagWaitAny, FuriWaitForever);
    mu_assert(flag == NFC_TEST_FLAG_WORKER_DONE, "Wrong thread flag");
    nfc_poller_stop(mfc_poller);
    nfc_poller_free(mfc_poller);

    mu_assert(
        context.state == NfcTestMfClassicSendFrameTestStateSuccess, "Wrong test state at the end");

    bit_buffer_free(context.tx_buf);
    bit_buffer_free(context.rx_buf);
    nfc_listener_stop(mfc_listener);
    nfc_listener_free(mfc_listener);
    nfc_device_free(nfc_device);
    nfc_free(listener);
    nfc_free(poller);
}

MU_TEST(mf_classic_dict_test) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(storage_common_stat(storage, NFC_APP_MF_CLASSIC_DICT_UNIT_TEST_PATH, NULL) == FSE_OK) {
        mu_assert(
            storage_simply_remove(storage, NFC_APP_MF_CLASSIC_DICT_UNIT_TEST_PATH),
            "Remove test dict failed");
    }

    KeysDict* dict = keys_dict_alloc(
        NFC_APP_MF_CLASSIC_DICT_UNIT_TEST_PATH, KeysDictModeOpenAlways, sizeof(MfClassicKey));
    mu_assert(dict != NULL, "keys_dict_alloc() failed");

    size_t dict_keys_total = keys_dict_get_total_keys(dict);
    mu_assert(dict_keys_total == 0, "keys_dict_keys_total() failed");

    const uint32_t test_key_num = 30;
    MfClassicKey* key_arr_ref = malloc(test_key_num * sizeof(MfClassicKey));
    for(size_t i = 0; i < test_key_num; i++) {
        furi_hal_random_fill_buf(key_arr_ref[i].data, sizeof(MfClassicKey));
        mu_assert(
            keys_dict_add_key(dict, key_arr_ref[i].data, sizeof(MfClassicKey)), "add key failed");

        size_t dict_keys_total = keys_dict_get_total_keys(dict);
        mu_assert(dict_keys_total == (i + 1), "keys_dict_keys_total() failed");
    }

    keys_dict_free(dict);

    dict = keys_dict_alloc(
        NFC_APP_MF_CLASSIC_DICT_UNIT_TEST_PATH, KeysDictModeOpenAlways, sizeof(MfClassicKey));
    mu_assert(dict != NULL, "keys_dict_alloc() failed");

    dict_keys_total = keys_dict_get_total_keys(dict);
    mu_assert(dict_keys_total == test_key_num, "keys_dict_keys_total() failed");

    MfClassicKey key_dut = {};
    size_t key_idx = 0;
    while(keys_dict_get_next_key(dict, key_dut.data, sizeof(MfClassicKey))) {
        mu_assert(
            memcmp(key_arr_ref[key_idx].data, key_dut.data, sizeof(MfClassicKey)) == 0,
            "Loaded key data mismatch");
        key_idx++;
    }

    uint32_t delete_keys_idx[] = {1, 3, 9, 11, 19, 27};

    for(size_t i = 0; i < COUNT_OF(delete_keys_idx); i++) {
        MfClassicKey* key = &key_arr_ref[delete_keys_idx[i]];
        mu_assert(
            keys_dict_is_key_present(dict, key->data, sizeof(MfClassicKey)),
            "keys_dict_is_key_present() failed");
        mu_assert(
            keys_dict_delete_key(dict, key->data, sizeof(MfClassicKey)),
            "keys_dict_delete_key() failed");
    }

    dict_keys_total = keys_dict_get_total_keys(dict);
    mu_assert(
        dict_keys_total == test_key_num - COUNT_OF(delete_keys_idx),
        "keys_dict_keys_total() failed");

    keys_dict_free(dict);
    free(key_arr_ref);

    mu_assert(
        storage_simply_remove(storage, NFC_APP_MF_CLASSIC_DICT_UNIT_TEST_PATH),
        "Remove test dict failed");
}

static FelicaError
    felica_do_request_response(FelicaData* felica_data, const FelicaCardKey* card_key) {
    NfcDeviceData* nfc_device = nfc_device_alloc();

    FelicaError error = FelicaErrorNone;
    if(!nfc_device_load(nfc_device, EXT_PATH("unit_tests/nfc/Felica.nfc"))) {
        error = FelicaErrorNotPresent;
    } else {
        Nfc* poller = nfc_alloc();
        Nfc* listener = nfc_alloc();
        NfcListener* felica_listener = nfc_listener_alloc(
            listener, NfcProtocolFelica, nfc_device_get_data(nfc_device, NfcProtocolFelica));
        nfc_listener_start(felica_listener, NULL, NULL);

        error = felica_poller_sync_read(poller, felica_data, card_key);

        nfc_listener_stop(felica_listener);
        nfc_listener_free(felica_listener);

        nfc_free(listener);
        nfc_free(poller);
    }

    nfc_device_free(nfc_device);
    return error;
}

MU_TEST(felica_read) {
    FelicaData* felica_data = felica_alloc();
    FelicaError error = felica_do_request_response(felica_data, NULL);
    mu_assert(error == FelicaErrorNone, "felica_poller() failed");
    mu_assert(felica_data->data.fs.spad[4].SF1 == 0x01, "block[4].SF1 != 0x01");
    mu_assert(felica_data->data.fs.spad[4].SF2 == 0xB1, "block[4].SF2 != 0xB1");

    felica_free(felica_data);
}

// Minimal check of the FeliCa file format v3: no radio involved, just parsing
MU_TEST(felica_standard_file_test) {
    NfcDevice* nfc_device = nfc_device_alloc();
    mu_assert(
        nfc_device_load(nfc_device, EXT_PATH("unit_tests/nfc/Felica_Standard_with_keys.nfc")),
        "nfc_device_load() failed");

    const FelicaData* data = nfc_device_get_data(nfc_device, NfcProtocolFelica);
    mu_assert(data->workflow_type == FelicaStandard, "workflow_type != FelicaStandard");
    mu_assert(simple_array_get_count(data->systems) == 1, "system count != 1");

    const FelicaSystem* system = simple_array_cget(data->systems, 0);
    mu_assert(system->system_code == 0x0003, "system_code != 0x0003");
    mu_assert(system->key_version == 0x0001, "system key_version != 0x0001");

    mu_assert(simple_array_get_count(system->areas) == 1, "area count != 1");
    const FelicaArea* area = simple_array_cget(system->areas, 0);
    mu_assert(area->code == 0x0000, "area code != 0x0000");
    mu_assert(area->end_code == 0xFFFE, "area end_code != 0xFFFE");
    mu_assert(area->key_version == 0x0002, "area key_version != 0x0002");

    mu_assert(simple_array_get_count(system->services) == 2, "service count != 2");
    const FelicaService* service0 = simple_array_cget(system->services, 0);
    mu_assert(service0->code == 0x0009, "service[0] code != 0x0009");
    mu_assert(service0->key_version == 0x0003, "service[0] key_version != 0x0003");
    const FelicaService* service1 = simple_array_cget(system->services, 1);
    mu_assert(service1->code == 0x000B, "service[1] code != 0x000B");
    mu_assert(service1->key_version == 0x0004, "service[1] key_version != 0x0004");

    nfc_device_free(nfc_device);
}

// Version 2 files carry neither area end codes nor key versions
MU_TEST(felica_standard_v2_file_test) {
    NfcDevice* nfc_device = nfc_device_alloc();
    mu_assert(
        nfc_device_load(nfc_device, EXT_PATH("unit_tests/nfc/Felica_Standard_v2.nfc")),
        "nfc_device_load() failed");

    const FelicaData* data = nfc_device_get_data(nfc_device, NfcProtocolFelica);
    mu_assert(data->workflow_type == FelicaStandard, "workflow_type != FelicaStandard");
    mu_assert(simple_array_get_count(data->systems) == 1, "system count != 1");

    const FelicaSystem* system = simple_array_cget(data->systems, 0);
    mu_assert(system->system_code == 0x0003, "system_code != 0x0003");
    mu_assert(system->key_version == FELICA_KEY_VERSION_UNKNOWN, "system key_version is known");

    mu_assert(simple_array_get_count(system->areas) == 1, "area count != 1");
    const FelicaArea* area = simple_array_cget(system->areas, 0);
    mu_assert(area->code == 0x0000, "area code != 0x0000");
    mu_assert(area->end_code == FELICA_AREA_END_CODE_UNKNOWN, "area end_code is known");
    mu_assert(area->key_version == FELICA_KEY_VERSION_UNKNOWN, "area key_version is known");
    mu_assert(area->first_idx == 0, "area first_idx != 0");
    mu_assert(area->last_idx == 1, "area last_idx != 1");

    mu_assert(simple_array_get_count(system->services) == 2, "service count != 2");
    const FelicaService* service0 = simple_array_cget(system->services, 0);
    mu_assert(service0->code == 0x0009, "service[0] code != 0x0009");
    mu_assert(service0->key_version == FELICA_KEY_VERSION_UNKNOWN, "service[0] key_version known");
    const FelicaService* service1 = simple_array_cget(system->services, 1);
    mu_assert(service1->code == 0x000B, "service[1] code != 0x000B");
    mu_assert(service1->key_version == FELICA_KEY_VERSION_UNKNOWN, "service[1] key_version known");

    mu_assert(simple_array_get_count(system->public_blocks) == 2, "public block count != 2");

    nfc_device_free(nfc_device);
}

// Version 3 files stored the root area key version in the system field, so it is not trusted
MU_TEST(felica_standard_v3_file_test) {
    NfcDevice* nfc_device = nfc_device_alloc();
    mu_assert(
        nfc_device_load(nfc_device, EXT_PATH("unit_tests/nfc/Felica_Standard_v3.nfc")),
        "nfc_device_load() failed");

    const FelicaData* data = nfc_device_get_data(nfc_device, NfcProtocolFelica);
    mu_assert(data->workflow_type == FelicaStandard, "workflow_type != FelicaStandard");
    mu_assert(simple_array_get_count(data->systems) == 1, "system count != 1");

    const FelicaSystem* system = simple_array_cget(data->systems, 0);
    mu_assert(system->system_code == 0x0003, "system_code != 0x0003");
    mu_assert(system->key_version == FELICA_KEY_VERSION_UNKNOWN, "system key_version is known");

    mu_assert(simple_array_get_count(system->areas) == 1, "area count != 1");
    const FelicaArea* area = simple_array_cget(system->areas, 0);
    mu_assert(area->code == 0x0000, "area code != 0x0000");
    mu_assert(area->end_code == 0xFFFE, "area end_code != 0xFFFE");
    mu_assert(area->key_version == 0x0002, "area key_version != 0x0002");

    mu_assert(simple_array_get_count(system->services) == 2, "service count != 2");
    const FelicaService* service0 = simple_array_cget(system->services, 0);
    mu_assert(service0->key_version == 0x0003, "service[0] key_version != 0x0003");
    const FelicaService* service1 = simple_array_cget(system->services, 1);
    mu_assert(service1->key_version == 0x0004, "service[1] key_version != 0x0004");

    nfc_device_free(nfc_device);
}

MU_TEST(felica_standard_read) {
    NfcDeviceData* nfc_device = nfc_device_alloc();
    mu_assert(
        nfc_device_load(nfc_device, EXT_PATH("unit_tests/nfc/Felica_Standard_with_keys.nfc")),
        "nfc_device_load() failed");

    Nfc* poller = nfc_alloc();
    Nfc* listener = nfc_alloc();
    NfcListener* felica_listener = nfc_listener_alloc(
        listener, NfcProtocolFelica, nfc_device_get_data(nfc_device, NfcProtocolFelica));
    nfc_listener_start(felica_listener, NULL, NULL);

    FelicaData* felica_data = felica_alloc();
    FelicaError error = felica_poller_sync_read(poller, felica_data, NULL);

    nfc_listener_stop(felica_listener);
    nfc_listener_free(felica_listener);
    nfc_free(listener);
    nfc_free(poller);
    nfc_device_free(nfc_device);

    mu_assert(error == FelicaErrorNone, "felica_poller_sync_read() failed");
    mu_assert(simple_array_get_count(felica_data->systems) == 1, "system count != 1");

    const FelicaSystem* system = simple_array_cget(felica_data->systems, 0);
    mu_assert(system->key_version == 0x0001, "system key_version != 0x0001");

    mu_assert(simple_array_get_count(system->areas) == 1, "area count != 1");
    const FelicaArea* area = simple_array_cget(system->areas, 0);
    mu_assert(area->code == 0x0000, "area code != 0x0000");
    mu_assert(area->key_version == 0x0002, "area key_version != 0x0002");

    mu_assert(simple_array_get_count(system->services) == 2, "service count != 2");
    const FelicaService* service0 = simple_array_cget(system->services, 0);
    mu_assert(service0->code == 0x0009, "service[0] code != 0x0009");
    mu_assert(service0->key_version == 0x0003, "service[0] key_version != 0x0003");
    const FelicaService* service1 = simple_array_cget(system->services, 1);
    mu_assert(service1->code == 0x000B, "service[1] code != 0x000B");
    mu_assert(service1->key_version == 0x0004, "service[1] key_version != 0x0004");

    mu_assert(simple_array_get_count(system->public_blocks) == 2, "public block count != 2");

    const FelicaPublicBlock* pb0 = simple_array_cget(system->public_blocks, 0);
    mu_assert(pb0->service_code == 0x0009, "block[0] service_code != 0x0009");
    mu_assert(pb0->block_idx == 0, "block[0] block_idx != 0");
    static const uint8_t expected_block0[FELICA_DATA_BLOCK_SIZE] = {
        0x01,
        0x02,
        0x03,
        0x04,
        0x05,
        0x06,
        0x07,
        0x08,
        0x09,
        0x0A,
        0x0B,
        0x0C,
        0x0D,
        0x0E,
        0x0F,
        0x10};
    mu_assert(
        memcmp(pb0->block.data, expected_block0, FELICA_DATA_BLOCK_SIZE) == 0,
        "block[0] data mismatch");

    const FelicaPublicBlock* pb1 = simple_array_cget(system->public_blocks, 1);
    mu_assert(pb1->service_code == 0x000B, "block[1] service_code != 0x000B");
    mu_assert(pb1->block_idx == 0, "block[1] block_idx != 0");
    static const uint8_t expected_block1[FELICA_DATA_BLOCK_SIZE] = {
        0xA1,
        0xA2,
        0xA3,
        0xA4,
        0xA5,
        0xA6,
        0xA7,
        0xA8,
        0xA9,
        0xAA,
        0xAB,
        0xAC,
        0xAD,
        0xAE,
        0xAF,
        0xB0};
    mu_assert(
        memcmp(pb1->block.data, expected_block1, FELICA_DATA_BLOCK_SIZE) == 0,
        "block[1] data mismatch");

    felica_free(felica_data);
}

MU_TEST(felica_read_auth) {
    FelicaData* felica_data = felica_alloc();
    FelicaCardKey card_key;
    memset(card_key.data, 0xFF, FELICA_DATA_BLOCK_SIZE);

    FelicaError error = felica_do_request_response(felica_data, &card_key);
    mu_assert(error == FelicaErrorNone, "felica_poller() failed");
    mu_assert(felica_data->data.fs.spad[4].SF1 == 0x00, "block[4].SF1 != 0x00");
    mu_assert(felica_data->data.fs.spad[4].SF2 == 0x00, "block[4].SF2 != 0x00");

    felica_free(felica_data);
}

MU_TEST(slix_file_with_capabilities_test) {
    NfcDevice* nfc_device_missed_cap = nfc_device_alloc();
    mu_assert(
        nfc_device_load(nfc_device_missed_cap, EXT_PATH("unit_tests/nfc/Slix_cap_missed.nfc")),
        "nfc_device_load() failed\r\n");

    NfcDevice* nfc_device_default_cap = nfc_device_alloc();
    mu_assert(
        nfc_device_load(nfc_device_default_cap, EXT_PATH("unit_tests/nfc/Slix_cap_default.nfc")),
        "nfc_device_load() failed\r\n");

    mu_assert(
        nfc_device_is_equal(nfc_device_missed_cap, nfc_device_default_cap),
        "nfc_device_is_equal() failed\r\n");

    nfc_device_free(nfc_device_default_cap);
    nfc_device_free(nfc_device_missed_cap);
}

NfcCommand slix_poller_set_password_callback(NfcGenericEventEx event, void* context) {
    furi_check(event.poller);
    furi_check(event.parent_event_data);
    furi_check(context);

    NfcCommand command = NfcCommandContinue;
    Iso15693_3PollerEvent* iso15_event = event.parent_event_data;
    SlixPoller* poller = event.poller;
    NfcTestSlixPollerSetPasswordContext* slix_ctx = context;

    if(iso15_event->type == Iso15693_3PollerEventTypeReady) {
        iso15693_3_copy(
            poller->data->iso15693_3_data, iso15693_3_poller_get_data(poller->iso15693_3_poller));

        if(slix_ctx->state == NfcTestSlixPollerSetPasswordStateGetRandomNumber) {
            slix_ctx->error = slix_poller_get_random_number(poller, &slix_ctx->random_number);
            if(slix_ctx->error != SlixErrorNone) {
                furi_thread_flags_set(slix_ctx->thread_id, NFC_TEST_FLAG_WORKER_DONE);
                command = NfcCommandStop;
            } else {
                slix_ctx->state = NfcTestSlixPollerSetPasswordStateSetPassword;
            }
        } else if(slix_ctx->state == NfcTestSlixPollerSetPasswordStateSetPassword) {
            slix_ctx->error = slix_poller_set_password(
                poller, SlixPasswordTypeRead, slix_ctx->password, slix_ctx->random_number);
            furi_thread_flags_set(slix_ctx->thread_id, NFC_TEST_FLAG_WORKER_DONE);
            command = NfcCommandStop;
        }
    } else {
        slix_ctx->error = slix_process_iso15693_3_error(iso15_event->data->error);
        furi_thread_flags_set(slix_ctx->thread_id, NFC_TEST_FLAG_WORKER_DONE);
        command = NfcCommandStop;
    }

    return command;
}

static void slix_set_password_test(const char* file_path, SlixPassword pass, bool correct_pass) {
    FURI_LOG_I(TAG, "Testing file: %s", file_path);

    Nfc* poller = nfc_alloc();
    Nfc* listener = nfc_alloc();

    NfcDevice* nfc_device = nfc_device_alloc();
    mu_assert(nfc_device_load(nfc_device, file_path), "nfc_device_load() failed\r\n");

    const SlixData* slix_data = nfc_device_get_data(nfc_device, NfcProtocolSlix);
    NfcListener* slix_listener = nfc_listener_alloc(listener, NfcProtocolSlix, slix_data);
    nfc_listener_start(slix_listener, NULL, NULL);

    SlixCapabilities slix_capabilities = slix_data->capabilities;

    NfcPoller* slix_poller = nfc_poller_alloc(poller, NfcProtocolSlix);

    NfcTestSlixPollerSetPasswordContext slix_poller_context = {
        .thread_id = furi_thread_get_current_id(),
        .state = NfcTestSlixPollerSetPasswordStateGetRandomNumber,
        .password = pass,
        .error = SlixErrorNone,
    };

    nfc_poller_start_ex(slix_poller, slix_poller_set_password_callback, &slix_poller_context);

    uint32_t flag =
        furi_thread_flags_wait(NFC_TEST_FLAG_WORKER_DONE, FuriFlagWaitAny, FuriWaitForever);
    mu_assert(flag == NFC_TEST_FLAG_WORKER_DONE, "Wrong thread flag\r\n");

    nfc_poller_stop(slix_poller);
    nfc_poller_free(slix_poller);
    nfc_listener_stop(slix_listener);
    nfc_listener_free(slix_listener);

    mu_assert(
        slix_poller_context.state == NfcTestSlixPollerSetPasswordStateSetPassword,
        "Poller failed before setting password\r\n");

    if((slix_capabilities == SlixCapabilitiesAcceptAllPasswords) || (correct_pass)) {
        mu_assert(slix_poller_context.error == SlixErrorNone, "Failed to set password\r\n");
    } else {
        mu_assert(
            slix_poller_context.error == SlixErrorTimeout,
            "Must have received SlixErrorTimeout\r\n");
    }

    nfc_device_free(nfc_device);
    nfc_free(listener);
    nfc_free(poller);
}

MU_TEST(slix_set_password_default_cap_correct_pass) {
    slix_set_password_test(EXT_PATH("unit_tests/nfc/Slix_cap_default.nfc"), 0x00000000, true);
}

MU_TEST(slix_set_password_default_cap_incorrect_pass) {
    slix_set_password_test(EXT_PATH("unit_tests/nfc/Slix_cap_default.nfc"), 0x12341234, false);
}

MU_TEST(slix_set_password_access_all_passwords_cap) {
    slix_set_password_test(
        EXT_PATH("unit_tests/nfc/Slix_cap_accept_all_pass.nfc"), 0x12341234, false);
}

// Reference values from Proxmark3's common/legic_prng.c and common/crc.c.
static uint32_t legic_prime_test_keystream(uint8_t iv, int skip, uint8_t bits) {
    legic_prime_prng_init(iv);
    legic_prime_prng_forward(skip);
    uint32_t value = 0;
    for(uint8_t i = 0; i < bits; i++) {
        value |= (uint32_t)legic_prime_prng_get_bit() << i;
        legic_prime_prng_forward(1);
    }
    return value;
}

MU_TEST(legic_prime_prng_test) {
    mu_assert_int_eq(0x82B50098, legic_prime_test_keystream(0x55, 0, 32));
    mu_assert_int_eq(0xAA13D912, legic_prime_test_keystream(0x01, 0, 32));
    mu_assert_int_eq(0x3D7AD81F, legic_prime_test_keystream(0x7F, 0, 32));
    mu_assert_int_eq(0x0093, legic_prime_test_keystream(0x55, 100, 16));
}

// Proxmark3 armsrc/legicrfsim.c rx_frame(), in its 212 kHz ticks (64 fc):
// starting 2 ticks before the reference, step once per 21-tick slot until
// the slot that holds the frame's first pause.
static uint32_t legic_prime_test_sim_idle_steps(uint32_t pause_tick) {
    int32_t last_frame_end = -2;
    uint32_t steps = 0;
    do {
        last_frame_end += 21;
        steps++;
    } while((int32_t)pause_tick > last_frame_end);
    return steps;
}

MU_TEST(legic_prime_listener_steps_test) {
    // legicrfsim.c only sees whole ticks.
    const uint32_t gap_ticks = LEGIC_PRIME_LISTENER_SESSION_GAP_FC / LEGIC_PRIME_LISTENER_CHIP_FC;
    for(uint32_t tick = 0; tick <= gap_ticks; tick++) {
        mu_assert_int_eq(
            legic_prime_test_sim_idle_steps(tick),
            legic_prime_listener_idle_steps(tick * LEGIC_PRIME_LISTENER_CHIP_FC));
    }

    // The reader's next frame 188.8-287.9 µs after the reply end takes 3 steps:
    // PM3 reader ~235 µs, Flipper reader ~266 µs (260 µs + IRQ pin lag).
    mu_assert_int_eq(2, legic_prime_listener_idle_steps(2560));
    mu_assert_int_eq(3, legic_prime_listener_idle_steps(2561));
    mu_assert_int_eq(3, legic_prime_listener_idle_steps(3187));
    mu_assert_int_eq(3, legic_prime_listener_idle_steps(3607));
    mu_assert_int_eq(3, legic_prime_listener_idle_steps(3904));
    mu_assert_int_eq(4, legic_prime_listener_idle_steps(3905));
    // Chained READ 221.8 µs after the reader's ACK, counted from one slot after it.
    mu_assert_int_eq(2, legic_prime_listener_idle_steps(1664));

    // Past the session gap (or a frame before the reference, which reads as
    // a gap of about a minute) the session is over.
    mu_assert_int_eq(202, legic_prime_listener_idle_steps(LEGIC_PRIME_LISTENER_SESSION_GAP_FC));
    mu_assert_int_eq(0, legic_prime_listener_idle_steps(LEGIC_PRIME_LISTENER_SESSION_GAP_FC + 1));
    mu_assert_int_eq(0, legic_prime_listener_idle_steps(UINT32_MAX));
}

MU_TEST(legic_prime_crc_test) {
    // CRC-8/LEGIC check value, as documented in Proxmark3's common/crc.c.
    mu_assert_int_eq(0xC6, legic_prime_crc8((const uint8_t*)"123456789", 9));

    const uint8_t uid1[] = {0x81, 0xAB, 0x2C, 0x87};
    const uint8_t uid2[] = {0x11, 0x22, 0x33, 0x44};
    const uint8_t uid3[] = {0xA1, 0xB2, 0xC3, 0xD4};
    mu_assert_int_eq(0x2D, legic_prime_crc8(uid1, sizeof(uid1)));
    mu_assert_int_eq(0xD4, legic_prime_crc8(uid2, sizeof(uid2)));
    mu_assert_int_eq(0xFD, legic_prime_crc8(uid3, sizeof(uid3)));

    // READ command (address << 1 | 1) or WRITE (address << 1), then the byte.
    mu_assert_int_eq(0x4, legic_prime_crc4((5 << 1) | 1, 9, 0x2D));
    mu_assert_int_eq(0x3, legic_prime_crc4((0 << 1) | 1, 6, 0x81));
    mu_assert_int_eq(0x7, legic_prime_crc4(600 << 1, 11, 0xA1));
    // The last MIM1024 address uses all 11 command bits.
    mu_assert_int_eq(0xD, legic_prime_crc4(1023 << 1, 11, 0xA5));
}

// On-air frames and reply keystreams for IV 0x55 on a MIM256 (9-bit
// commands), as Proxmark3's reader builds them (armsrc/legicrf.c
// setup_phase(), read_byte(), write_byte() and rx_ack() on
// common/legic_prng.c).
MU_TEST(legic_prime_frame_test) {
    LegicPrimeExchange ex;

    legic_prime_poller_build_iv(&ex, 0x55);
    mu_assert_int_eq(0x55, ex.tx);
    mu_assert_int_eq(7, ex.tx_bits);
    mu_assert_int_eq(6, ex.rx_bits);
    // MIM256's type code 0x1D arrives as 0x3B.
    mu_assert_int_eq(0x26, ex.ks);

    // The ACK, the first READ chained behind it, then READs behind replies.
    legic_prime_poller_build(&ex, 0x39, 6, LEGIC_PRIME_STEPS_AFTER_REPLY, 0);
    mu_assert_int_eq(0x19, ex.tx);
    mu_assert_int_eq(0, ex.ks);
    legic_prime_poller_build(&ex, (0 << 1) | 1, 9, LEGIC_PRIME_STEPS_AFTER_READER_ACK, 12);
    mu_assert_int_eq(0x057, ex.tx);
    mu_assert_int_eq(0x352, ex.ks);
    legic_prime_poller_build(&ex, (1 << 1) | 1, 9, LEGIC_PRIME_STEPS_AFTER_REPLY, 12);
    mu_assert_int_eq(0x17C, ex.tx);
    mu_assert_int_eq(0x387, ex.ks);
    legic_prime_poller_build(&ex, (2 << 1) | 1, 9, LEGIC_PRIME_STEPS_AFTER_REPLY, 12);
    mu_assert_int_eq(0x0FA, ex.tx);
    mu_assert_int_eq(0x905, ex.ks);

    // IV, ACK and READ 0, then WRITE 0x16 = A5 behind the READ's reply, its
    // ACK in window 33 and WRITE 0x15 = 5A chained behind that ACK.
    legic_prime_poller_build_iv(&ex, 0x55);
    legic_prime_poller_build(&ex, 0x39, 6, LEGIC_PRIME_STEPS_AFTER_REPLY, 0);
    legic_prime_poller_build(&ex, (0 << 1) | 1, 9, LEGIC_PRIME_STEPS_AFTER_READER_ACK, 12);
    const uint32_t write1 = legic_prime_poller_write_frame(0x16, 0xA5, 9);
    mu_assert_int_eq(0x034A2C, write1);
    legic_prime_poller_build(&ex, write1, 21, LEGIC_PRIME_STEPS_AFTER_REPLY, 0);
    mu_assert_int_eq(0x1F7153, ex.tx);
    const uint32_t write2 = legic_prime_poller_write_frame(0x15, 0x5A, 9);
    mu_assert_int_eq(0x1CB42A, write2);
    legic_prime_poller_build(&ex, write2, 21, 33 + LEGIC_PRIME_WRITE_STEPS_AFTER_ACK, 0);
    mu_assert_int_eq(0x0CB42E, ex.tx);
}

// Generated MIM256 IM-S image with two segments; the second one has a
// corrupted header CRC.  Proxmark3 `hf legic info`, reading it emulated by
// the Flipper, reports the same two segments (len 40 WRP 10 WRC 3 CRC ok;
// len 30 RD CRC fail).
static const uint8_t legic_prime_test_ims256[] = {
    0x11, 0x22, 0x33, 0x44, 0xD4, 0x05, 0x00, 0x9F, 0xFF, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0xFC, 0x94, 0xDE, 0xE4, 0x92, 0xC4, 0xC5, 0xC6, 0xC7, 0xC0,
    0xC1, 0xC2, 0xC3, 0xCC, 0xCD, 0xCE, 0xCF, 0xC8, 0xC9, 0xCA, 0xCB, 0xC4, 0xC5, 0xC6, 0xC7, 0xC0,
    0xC1, 0xC2, 0xC3, 0xCC, 0xCD, 0xCE, 0xCF, 0xC8, 0xC9, 0xCA, 0xCB, 0xC4, 0xC5, 0xC6, 0xCA, 0x14,
    0xD4, 0x54, 0x2D, 0xF4, 0xF5, 0xF6, 0xF7, 0xF0, 0xF1, 0xF2, 0xF3, 0xFC, 0xFD, 0xFE, 0xFF, 0xF8,
    0xF9, 0xFA, 0xFB, 0xF4, 0xF5, 0xF6, 0xF7, 0xF0, 0xF1, 0xF2, 0xF3, 0xFC,
};

static LegicPrimeData* legic_prime_test_card(void) {
    LegicPrimeData* data = legic_prime_alloc();
    legic_prime_set_type(data, LegicPrimeTypeMim256);
    // The unused area is 00 obfuscated with the MCC (0xD4).
    memset(data->data, 0xD4, legic_prime_get_mem_size(data->type));
    memcpy(data->data, legic_prime_test_ims256, sizeof(legic_prime_test_ims256));
    return data;
}

// Checks the two segments of legic_prime_test_card(), whatever its UID.
static void legic_prime_test_segments(const LegicPrimeData* data) {
    LegicPrimeSegment seg = {};

    mu_assert(legic_prime_get_next_segment(data, &seg), "segment 1 missing\r\n");
    mu_assert_int_eq(22, seg.offset);
    mu_assert_int_eq(40, seg.length);
    mu_assert_int_eq(27, seg.payload_offset);
    mu_assert_int_eq(35, seg.payload_length);
    mu_assert(seg.valid && !seg.last && !seg.rd && seg.crc_ok, "segment 1 flags\r\n");
    mu_assert_int_eq(10, seg.wrp);
    mu_assert_int_eq(3, seg.wrc);

    mu_assert(legic_prime_get_next_segment(data, &seg), "segment 2 missing\r\n");
    mu_assert_int_eq(62, seg.offset);
    mu_assert_int_eq(30, seg.length);
    mu_assert(seg.valid && seg.last && seg.rd && !seg.crc_ok, "segment 2 flags\r\n");
    mu_assert_int_eq(0, seg.wrp);
    mu_assert_int_eq(0, seg.wrc);

    mu_assert(!legic_prime_get_next_segment(data, &seg), "segment past the last one\r\n");
}

MU_TEST(legic_prime_layout_test) {
    LegicPrimeData* data = legic_prime_test_card();

    mu_assert(legic_prime_verify_mcc(data), "MCC\r\n");
    mu_assert_int_eq(5, legic_prime_get_dcf(data));
    mu_assert_int_eq(LegicPrimeTokenTypeImSegmented, legic_prime_get_token_type(data));
    legic_prime_test_segments(data);

    // Other token types, from the DCF and bytes 7-8.
    LegicPrimeSegment seg = {};
    data->data[5] = 0xFF;
    data->data[6] = 0xFF;
    mu_assert_int_eq(LegicPrimeTokenTypeNewMedia, legic_prime_get_token_type(data));
    mu_assert(!legic_prime_get_next_segment(data, &seg), "segments on new media\r\n");
    data->data[5] = 0x34;
    data->data[6] = 0x12;
    data->data[7] = 0x25;
    mu_assert_int_eq(LegicPrimeTokenTypeIm, legic_prime_get_token_type(data));
    data->data[5] = 0x10;
    data->data[6] = 0xF0;
    mu_assert_int_eq(LegicPrimeTokenTypeIam, legic_prime_get_token_type(data));
    data->data[5] = 0x40;
    mu_assert_int_eq(LegicPrimeTokenTypeSam, legic_prime_get_token_type(data));
    data->data[5] = 0x75;
    mu_assert_int_eq(LegicPrimeTokenTypeGam, legic_prime_get_token_type(data));
    data->data[6] = 0xEC;
    mu_assert_int_eq(LegicPrimeTokenTypeXam, legic_prime_get_token_type(data));

    legic_prime_free(data);
}

MU_TEST(legic_prime_bad_segment_test) {
    LegicPrimeData* data = legic_prime_test_card();
    LegicPrimeSegment seg = {};
    mu_assert(legic_prime_get_next_segment(data, &seg), "segment 1 missing\r\n");
    const LegicPrimeSegment first = seg;

    // Segment 2's header (at 62) gets length 3, shorter than a header.
    data->data[62] = 3 ^ data->data[4];
    mu_assert(!legic_prime_get_next_segment(data, &seg), "malformed segment 2\r\n");
    mu_assert_mem_eq(&first, &seg, sizeof(seg));

    legic_prime_free(data);
}

MU_TEST(legic_prime_set_uid_test) {
    const uint8_t uid[] = {0xA1, 0xB2, 0xC3, 0xD4};
    const uint8_t old_uid[] = {0x11, 0x22, 0x33, 0x44};

    // As nfc_device_load() does it, on a reset card.
    LegicPrimeData* data = legic_prime_alloc();
    mu_assert_int_eq(LegicPrimeTypeMim22, data->type);
    mu_assert_int_eq(LEGIC_PRIME_MEM_SIZE_MIM22, legic_prime_get_mem_size(data->type));
    mu_assert(legic_prime_set_uid(data, uid, sizeof(uid)), "set_uid on a reset card\r\n");
    mu_assert_mem_eq(uid, data->data, sizeof(uid));
    mu_assert(legic_prime_verify_mcc(data), "MCC after set_uid on a reset card\r\n");
    mu_assert(!legic_prime_set_uid(data, uid, 7), "set_uid with a 7-byte UID\r\n");
    legic_prime_free(data);

    LegicPrimeData* reference = legic_prime_test_card();
    data = legic_prime_test_card();
    mu_assert(legic_prime_set_uid(data, uid, sizeof(uid)), "set_uid\r\n");
    mu_assert_mem_eq(uid, data->data, sizeof(uid));
    mu_assert_int_eq(0xFD, data->data[4]);
    // The header area is not obfuscated.
    mu_assert_mem_eq(&reference->data[5], &data->data[5], 22 - 5);
    // Bytes from 22 on move from MCC D4 to FD, except the segment header
    // CRCs: segment 1's matches the new UID (Proxmark3 CRC8Legic), segment
    // 2's stays wrong.
    for(size_t i = 22; i < legic_prime_get_mem_size(data->type); i++) {
        if(i == 26 || i == 66) continue;
        mu_assert_int_eq(reference->data[i] ^ 0xD4 ^ 0xFD, data->data[i]);
    }
    mu_assert_int_eq(0xA8, data->data[26]);
    mu_assert_int_eq(0x17, data->data[66]);
    legic_prime_test_segments(data);

    // Back to the old UID: the original image.
    mu_assert(legic_prime_set_uid(data, old_uid, sizeof(old_uid)), "set_uid back\r\n");
    mu_assert(legic_prime_is_equal(reference, data), "set_uid round trip\r\n");

    legic_prime_free(reference);
    legic_prime_free(data);
}

MU_TEST(legic_prime_zero_init_test) {
    // A caller's own zeroed struct, as a stack `LegicPrimeData d = {0}` that a
    // failed read left untouched: it is the blank MIM22 a reset leaves, and
    // every reader takes it.  On the heap only to keep 1 KB off the stack.
    LegicPrimeData* zero = malloc(sizeof(LegicPrimeData));
    memset(zero, 0, sizeof(LegicPrimeData));
    LegicPrimeData* reset = legic_prime_alloc();

    mu_assert_int_eq(0, legic_prime_get_dcf(zero));
    mu_assert(legic_prime_is_equal(zero, reset), "zero-init equals reset\r\n");

    legic_prime_free(reset);
    free(zero);
}

MU_TEST(legic_prime_write_range_test) {
    // Bytes 0-4 never, the rest up to the end of the card.
    static const struct {
        uint16_t data_size;
        uint16_t offset;
        uint16_t size;
        bool writable;
    } writable_cases[] = {
        {256, 0, 1, false},     {256, 4, 1, false},       {256, 4, 2, false},
        {256, 5, 1, true},      {256, 5, 0, false},       {256, 6, 2, true},
        {256, 7, 1, true},      {22, 21, 1, true},        {22, 21, 2, false},
        {22, 22, 1, false},     {22, 5, 17, true},        {22, 5, 18, false},
        {256, 255, 1, true},    {256, 255, 2, false},     {256, 256, 1, false},
        {256, 5, 251, true},    {256, 5, 252, false},     {1024, 1023, 1, true},
        {1024, 1023, 2, false}, {1024, 1024, 1, false},   {1024, 5, 1019, true},
        {1024, 5, 1020, false}, {1024, 0xFFFF, 1, false}, {256, 5, 0xFFFF, false},
    };
    for(size_t i = 0; i < COUNT_OF(writable_cases); i++) {
        mu_assert(
            legic_prime_is_writable(
                writable_cases[i].data_size, writable_cases[i].offset, writable_cases[i].size) ==
                writable_cases[i].writable,
            "legic_prime_is_writable\r\n");
    }

    // The DCF is bytes 5-6.
    static const struct {
        uint16_t offset;
        uint16_t size;
        bool dcf;
    } dcf_cases[] = {
        {0, 4, false},
        {0, 5, false},
        {0, 6, true},
        {3, 2, false},
        {4, 1, false},
        {4, 2, true},
        {5, 1, true},
        {5, 2, true},
        {6, 1, true},
        {6, 2, true},
        {7, 1, false},
        {7, 2, false},
        {2, 100, true},
        {5, 0, false},
        {300, 4, false},
    };
    for(size_t i = 0; i < COUNT_OF(dcf_cases); i++) {
        mu_assert(
            legic_prime_is_dcf_write(dcf_cases[i].offset, dcf_cases[i].size) == dcf_cases[i].dcf,
            "legic_prime_is_dcf_write\r\n");
    }
}

MU_TEST(legic_prime_read_range_test) {
    // The requested range clamped to the card; size 0 is the whole card.
    static const struct {
        LegicPrimeType type;
        uint16_t offset;
        uint16_t size;
        uint16_t start;
        uint16_t end;
    } cases[] = {
        {LegicPrimeTypeMim256, 0, 0, 0, 256},
        {LegicPrimeTypeMim256, 22, 16, 22, 38},
        {LegicPrimeTypeMim256, 250, 16, 250, 256},
        {LegicPrimeTypeMim256, 300, 16, 256, 256},
        {LegicPrimeTypeMim256, 5, 0xFFFF, 5, 256},
        // A corrected type changes the card's size, and the range with it.
        {LegicPrimeTypeMim22, 0, 0, 0, 22},
        {LegicPrimeTypeMim22, 20, 16, 20, 22},
    };
    LegicPrimePoller poller = {};
    poller.data = legic_prime_alloc();
    poller.progress = malloc(LEGIC_PRIME_MEM_MAX);
    for(size_t i = 0; i < COUNT_OF(cases); i++) {
        legic_prime_set_type(poller.data, cases[i].type);
        poller.read_offset = cases[i].offset;
        poller.read_size = cases[i].size;
        // A previous read's state, which the reset must clear.
        poller.done_count = 7;
        poller.uid_known = true;
        poller.read_cursor = 3;
        memset(poller.progress, LEGIC_PRIME_POLLER_DONE, LEGIC_PRIME_MEM_MAX);
        memset(poller.data->data, 0xA5, sizeof(poller.data->data));

        legic_prime_poller_read_reset(&poller);
        mu_assert_int_eq(cases[i].start, poller.read_start);
        mu_assert_int_eq(cases[i].end, poller.read_end);
        mu_assert_int_eq(0, poller.done_count);
        mu_assert(!poller.uid_known, "UID still known after a reset\r\n");
        // The next read is of the first byte of the range.
        mu_assert_int_eq(cases[i].end - 1, poller.read_cursor);
        for(size_t a = 0; a < LEGIC_PRIME_MEM_MAX; a++) {
            mu_assert_int_eq(0, poller.progress[a]);
            mu_assert_int_eq(0, poller.data->data[a]);
        }
    }
    free(poller.progress);
    legic_prime_free(poller.data);
}

MU_TEST(legic_prime_session_cap_test) {
    // 32 times the clean session count (one per 320 bytes read, two agreeing
    // reads each, or 256 written, at least one) plus a resume budget: never
    // 0, however short the range.
    mu_assert_int_eq(40, legic_prime_poller_max_sessions(LegicPrimePollerModeRead, 0));
    mu_assert_int_eq(
        40, legic_prime_poller_max_sessions(LegicPrimePollerModeRead, LEGIC_PRIME_MEM_SIZE_MIM22));
    mu_assert_int_eq(
        40,
        legic_prime_poller_max_sessions(LegicPrimePollerModeRead, LEGIC_PRIME_MEM_SIZE_MIM256));
    mu_assert_int_eq(
        136,
        legic_prime_poller_max_sessions(LegicPrimePollerModeRead, LEGIC_PRIME_MEM_SIZE_MIM1024));
    mu_assert_int_eq(40, legic_prime_poller_max_sessions(LegicPrimePollerModeWrite, 16));
    mu_assert_int_eq(136, legic_prime_poller_max_sessions(LegicPrimePollerModeWrite, 1000));
}

static void legic_prime_file_test(LegicPrimeType type) {
    NfcDevice* nfc_device = nfc_device_alloc();
    LegicPrimeData* data = legic_prime_alloc();

    legic_prime_set_type(data, type);
    // Past the card's size too: only that many bytes are saved and compared.
    furi_hal_random_fill_buf(data->data, sizeof(data->data));
    nfc_device_set_data(nfc_device, NfcProtocolLegicPrime, data);
    nfc_test_save_and_load(nfc_device);

    legic_prime_free(data);
    nfc_device_free(nfc_device);
}

MU_TEST(legic_prime_mim22_file_test) {
    legic_prime_file_test(LegicPrimeTypeMim22);
}

MU_TEST(legic_prime_mim256_file_test) {
    legic_prime_file_test(LegicPrimeTypeMim256);
}

MU_TEST(legic_prime_mim1024_file_test) {
    legic_prime_file_test(LegicPrimeTypeMim1024);
}

MU_TEST_SUITE(nfc) {
    nfc_test_alloc();

    MU_RUN_TEST(iso14443_3a_reader);
    MU_RUN_TEST(mf_ultralight_11_reader);
    MU_RUN_TEST(mf_ultralight_21_reader);
    MU_RUN_TEST(ntag_215_reader);
    MU_RUN_TEST(ntag_216_reader);
    MU_RUN_TEST(ntag_213_locked_reader);
    MU_RUN_TEST(mf_ultralight_c_reader);

    MU_RUN_TEST(mf_ultralight_write);

    MU_RUN_TEST(iso14443_3a_4b_file_test);
    MU_RUN_TEST(iso14443_3a_7b_file_test);

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

    MU_RUN_TEST(mf_classic_reader);
    MU_RUN_TEST(mf_classic_write);
    MU_RUN_TEST(mf_classic_value_block);
    MU_RUN_TEST(mf_classic_send_frame_test);
    MU_RUN_TEST(mf_classic_dict_test);
    MU_RUN_TEST(felica_read);
    MU_RUN_TEST(felica_read_auth);
    MU_RUN_TEST(felica_standard_file_test);
    MU_RUN_TEST(felica_standard_v2_file_test);
    MU_RUN_TEST(felica_standard_v3_file_test);
    MU_RUN_TEST(felica_standard_read);

    MU_RUN_TEST(slix_file_with_capabilities_test);
    MU_RUN_TEST(slix_set_password_default_cap_correct_pass);
    MU_RUN_TEST(slix_set_password_default_cap_incorrect_pass);
    MU_RUN_TEST(slix_set_password_access_all_passwords_cap);

    MU_RUN_TEST(legic_prime_prng_test);
    MU_RUN_TEST(legic_prime_listener_steps_test);
    MU_RUN_TEST(legic_prime_crc_test);
    MU_RUN_TEST(legic_prime_frame_test);
    MU_RUN_TEST(legic_prime_layout_test);
    MU_RUN_TEST(legic_prime_bad_segment_test);
    MU_RUN_TEST(legic_prime_set_uid_test);
    MU_RUN_TEST(legic_prime_zero_init_test);
    MU_RUN_TEST(legic_prime_write_range_test);
    MU_RUN_TEST(legic_prime_read_range_test);
    MU_RUN_TEST(legic_prime_session_cap_test);
    MU_RUN_TEST(legic_prime_mim22_file_test);
    MU_RUN_TEST(legic_prime_mim256_file_test);
    MU_RUN_TEST(legic_prime_mim1024_file_test);

    nfc_test_free();
}

int run_minunit_test_nfc(void) {
    MU_RUN_SUITE(nfc);
    return MU_EXIT_CODE;
}

TEST_API_DEFINE(run_minunit_test_nfc)
