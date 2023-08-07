#include <furi.h>
#include <lib/subghz/transmitter.h>
#include <applications/drivers/subghz/cc1101_ext/cc1101_ext_interconnect.h>
#include <lib/subghz/devices/cc1101_int/cc1101_int_interconnect.h>
#include <flipper_format/flipper_format_i.h>
#include <lib/subghz/protocols/protocol_items.h>
#include <lib/subghz/devices/devices.h>

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

static void
    set_princeton(FlipperFormat* flipper_format, uint32_t key, uint32_t te, uint32_t repeat) {
    FuriString* princeton_settings = furi_string_alloc_printf(
        "Protocol: Princeton\n"
        "Bit: 24\n"
        "Key: 00 00 00 00 00 %02X %02X %02X\n"
        "TE: %lu\n"
        "Repeat: %lu\n",
        (uint8_t)((key >> 16) & 0xFFU),
        (uint8_t)((key >> 8) & 0xFFU),
        (uint8_t)(key & 0xFFU),
        te,
        repeat);
    Stream* stream = flipper_format_get_raw_stream(flipper_format);
    stream_clean(stream);
    stream_write_cstring(stream, furi_string_get_cstr(princeton_settings));
    stream_seek(stream, 0, StreamOffsetFromStart);
    furi_string_free(princeton_settings);
}

static void send_princeton(uint32_t key, bool use_external_radio) {
    uint32_t frequency = 433920000;
    uint32_t te = 350;
    uint32_t repeat = 5;
    bool enable_5v = use_external_radio;
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
    SubGhzTransmitter* transmitter = subghz_transmitter_alloc_init(environment, "Princeton");
    FlipperFormat* flipper_format = flipper_format_string_alloc();
    set_princeton(flipper_format, key, te, repeat);
    subghz_transmitter_deserialize(transmitter, flipper_format);
    subghz_devices_begin(device);
    subghz_devices_reset(device);
    subghz_devices_load_preset(device, FuriHalSubGhzPresetOok650Async, NULL);
    frequency = subghz_devices_set_frequency(device, frequency);

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

int princeton_send_app(char* p) {
    UNUSED(p);

    FURI_LOG_I("princeton_send", "Sending key with internal radio...");
    send_princeton(0x938475, false);
    FURI_LOG_I("princeton_send", "Sent key.");

    FURI_LOG_I("princeton_send", "Sending key with external radio...");
    send_princeton(0x194857, true);
    FURI_LOG_I("princeton_send", "Sent key?");

    return 0;
}