#include <furi.h>
#include <furi_hal.h>
#include "../minunit.h"
#include <lib/subghz/receiver.h>
#include <lib/subghz/transmitter.h>
#include <lib/subghz/subghz_keystore.h>
#include <lib/subghz/subghz_file_encoder_worker.h>

#define TAG "SubGhz TEST"
#define KEYSTORE_DIR_NAME "/ext/subghz/assets/keeloq_mfcodes"

static SubGhzEnvironment* environment_handler;
static SubGhzReceiver* receiver_handler;
//static SubGhzTransmitter* transmitter_handler;
static SubGhzFileEncoderWorker* file_worker_encoder_handler;
static bool subghz_test_decoder_ret = false;

static void subghz_test_rx_callback(
    SubGhzReceiver* receiver,
    SubGhzProtocolDecoderBase* decoder_base,
    void* context) {
    string_t text;
    string_init(text);
    subghz_protocol_decoder_base_get_string(decoder_base, text);
    subghz_receiver_reset(receiver);
    FURI_LOG_I(TAG, "%s", string_get_cstr(text));
    string_clear(text);
    subghz_test_decoder_ret = true;
}

static void subghz_test_init(void) {
    environment_handler = subghz_environment_alloc();

    receiver_handler = subghz_receiver_alloc_init(environment_handler);
    subghz_receiver_set_filter(receiver_handler, SubGhzProtocolFlag_Decodable);
    subghz_receiver_set_rx_callback(receiver_handler, subghz_test_rx_callback, NULL);

    file_worker_encoder_handler = subghz_file_encoder_worker_alloc();
    //transmitter_handler =
}

static void subghz_test_deinit(void) {
    subghz_receiver_free(receiver_handler);
    subghz_environment_free(environment_handler);

    if(subghz_file_encoder_worker_is_running(file_worker_encoder_handler)) {
        subghz_file_encoder_worker_stop(file_worker_encoder_handler);
    }
    subghz_file_encoder_worker_free(file_worker_encoder_handler);
}

static bool subghz_decode_test(const char* path, const char* name_decoder) {
    subghz_test_decoder_ret = false;

    if(subghz_file_encoder_worker_start(file_worker_encoder_handler, path)) {
        //the worker needs a file in order to open and read part of the file
        osDelay(100);
    }

    LevelDuration level_duration;
    while(1) {
        furi_hal_delay_us(500); //you need to have time to read from the file from the SD card
        level_duration =
            subghz_file_encoder_worker_get_level_duration(file_worker_encoder_handler);
        if(!level_duration_is_reset(level_duration)) {
            bool level = level_duration_get_level(level_duration);
            uint32_t duration = level_duration_get_duration(level_duration);
            subghz_receiver_decode(receiver_handler, level, duration);
        } else {
            break;
        }
    }
    furi_hal_delay_ms(10);
    return subghz_test_decoder_ret;
}

MU_TEST(subghz_keystore_test) {
    mu_assert(
        subghz_environment_load_keystore(environment_handler, KEYSTORE_DIR_NAME),
        "Test keystore error");
}

MU_TEST(subghz_decoder_test) {
    mu_assert(
        subghz_decode_test("/any/subghz/test.sub", "Princeton"), "Test decoder Princeton error");
}

MU_TEST_SUITE(subghz) {
    MU_SUITE_CONFIGURE(&subghz_test_init, &subghz_test_deinit);

    MU_RUN_TEST(subghz_keystore_test);
    MU_RUN_TEST(subghz_decoder_test);
}

int run_minunit_test_subghz() {
    MU_RUN_SUITE(subghz);
    return MU_EXIT_CODE;
}