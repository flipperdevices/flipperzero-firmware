#include "cc1101_ext_interconnect.h"
#include "cc1101_ext.h"

#define TAG "SubGhzDeviceCC1101Ext"

static bool subghz_device_cc1101_ext_interconnect_is_frequency_valid(uint32_t frequency) {
    bool ret = subghz_device_cc1101_ext_is_frequency_valid(frequency);
    if(!ret) {
        furi_crash("SubGhz: Incorrect frequency.");
    }
    return ret;
}

static uint32_t subghz_device_cc1101_ext_interconnect_set_frequency(uint32_t frequency) {
    subghz_device_cc1101_ext_interconnect_is_frequency_valid(frequency);
    return subghz_device_cc1101_ext_set_frequency(frequency);
}

static bool subghz_device_cc1101_ext_interconnect_start_async_tx(void* callback, void* context) {
    return subghz_device_cc1101_ext_start_async_tx(
        (SubGhzDeviceCC1101ExtCallback)callback, context);
}

static void subghz_device_cc1101_ext_interconnect_start_async_rx(void* callback, void* context) {
    subghz_device_cc1101_ext_start_async_rx(
        (SubGhzDeviceCC1101ExtCaptureCallback)callback, context);
}

const SubGhzDeviceInterconnect subghz_device_cc1101_ext_interconnect = {
    .begin = subghz_device_cc1101_ext_alloc,
    .end = subghz_device_cc1101_ext_free,
    //.check = subghz_device_cc1101_ext_check,
    .reset = subghz_device_cc1101_ext_reset,
    .sleep = subghz_device_cc1101_ext_sleep,
    .idle = subghz_device_cc1101_ext_idle,
    .load_preset = subghz_device_cc1101_ext_load_preset,
    .set_frequency = subghz_device_cc1101_ext_interconnect_set_frequency,
    .set_async_mirror_pin = subghz_device_cc1101_ext_set_async_mirror_pin,
    .set_tx = subghz_device_cc1101_ext_tx,
    .start_async_tx = subghz_device_cc1101_ext_interconnect_start_async_tx,
    .is_async_complete_tx = subghz_device_cc1101_ext_is_async_tx_complete,
    .stop_async_tx = subghz_device_cc1101_ext_stop_async_tx,
    .set_rx = subghz_device_cc1101_ext_rx,
    .start_async_rx = subghz_device_cc1101_ext_interconnect_start_async_rx,
    .stop_async_rx = subghz_device_cc1101_ext_stop_async_rx,
};

const SubGhzDevice subghz_device_cc1101_ext = {
    .name = SUBGHZ_DEVICE_CC1101_EXT_NAME,
    .interconnect = &subghz_device_cc1101_ext_interconnect,
};