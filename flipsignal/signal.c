#include <furi.h>
#include <lib/subghz/transmitter.h>
#include <flipper_format/flipper_format_i.h>
#include <lib/subghz/protocols/protocol_items.h>

#define FIRMWARE_SUBGHZ_UPDATED 1

#ifdef FIRMWARE_SUBGHZ_UPDATED
#include <applications/drivers/subghz/cc1101_ext/cc1101_ext_interconnect.h>
#include <lib/subghz/devices/cc1101_int/cc1101_int_interconnect.h>
#include <devices/devices.h>

static void subghz_radio_device_power_on() {
    uint8_t attempts = 5;
    while(--attempts > 0) {
        if(furi_hal_power_enable_otg()) break;
        furi_delay_ms(50);
    }
}

static void subghz_radio_device_power_off() {
    if(furi_hal_power_is_otg_enabled()) furi_hal_power_disable_otg();
}

static void set_signal(FuriString* sub_file_contents, FlipperFormat* flipper_format) {
    Stream* stream = flipper_format_get_raw_stream(flipper_format);
    stream_clean(stream);
    stream_write_cstring(stream, furi_string_get_cstr(sub_file_contents));
    stream_seek(stream, 0, StreamOffsetFromStart);
}

void send_signal(
    char* protocol,
    uint32_t frequency,
    FuriString* sub_file_contents,
    bool use_external_radio) {
    bool enable_5v = use_external_radio;
    FuriHalSubGhzPreset preset = FuriHalSubGhzPresetOok650Async;
    const SubGhzDevice* device;

    subghz_devices_init();
    if(use_external_radio) {
        if(enable_5v) {
            subghz_radio_device_power_on();
        }
        device = subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_EXT_NAME);
    } else {
        device = subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_INT_NAME);
    }

    SubGhzEnvironment* environment = subghz_environment_alloc();
    subghz_environment_set_protocol_registry(environment, (void*)&subghz_protocol_registry);
    SubGhzTransmitter* transmitter = subghz_transmitter_alloc_init(environment, protocol);
    FlipperFormat* flipper_format = flipper_format_string_alloc();
    set_signal(sub_file_contents, flipper_format);
    subghz_transmitter_deserialize(transmitter, flipper_format);
    subghz_devices_begin(device);
    subghz_devices_reset(device);
    subghz_devices_load_preset(device, preset, NULL);
    frequency = subghz_devices_set_frequency(device, frequency);

    // subghz_devices_set_async_mirror_pin -- blink the board status LED with the signal data?

    // Send
    furi_hal_power_suppress_charge_enter();
    if(subghz_devices_start_async_tx(device, subghz_transmitter_yield, transmitter)) {
        while(!(subghz_devices_is_async_complete_tx(device))) {
            furi_delay_ms(100);
        }

        subghz_devices_stop_async_tx(device);
    }

    // Cleanup
    subghz_devices_sleep(device);
    subghz_devices_end(device);
    subghz_devices_deinit();

    if(enable_5v) {
        subghz_radio_device_power_off();
    }
    furi_hal_power_suppress_charge_exit();
    flipper_format_free(flipper_format);
    subghz_transmitter_free(transmitter);
    subghz_environment_free(environment);
}
#else
void send_signal(
    char* protocol,
    uint32_t frequency,
    FuriString* sub_file_contents,
    bool use_external_radio) {
    FURI_LOG_D("FlipSignal", "TODO: Send %s signal", protocol);
    UNUSED(use_external_radio);
    UNUSED(frequency);
    UNUSED(sub_file_contents);
}
#endif
