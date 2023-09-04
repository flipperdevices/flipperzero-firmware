#include "rolling_flaws_send_keeloq.h"

#include <lib/subghz/transmitter.h>
#include <lib/subghz/devices/cc1101_int/cc1101_int_interconnect.h>
#include <lib/subghz/protocols/protocol_items.h>
#include <lib/subghz/devices/devices.h>

#ifdef TAG
#undef TAG
#endif
#define TAG "RollingFlawsSendKeeloq"

static SubGhzEnvironment* load_environment() {
    SubGhzEnvironment* environment = subghz_environment_alloc();
    subghz_environment_load_keystore(environment, SUBGHZ_KEYSTORE_DIR_NAME);
    subghz_environment_load_keystore(environment, SUBGHZ_KEYSTORE_DIR_USER_NAME);
    subghz_environment_set_came_atomo_rainbow_table_file_name(
        environment, SUBGHZ_CAME_ATOMO_DIR_NAME);
    subghz_environment_set_alutech_at_4n_rainbow_table_file_name(
        environment, SUBGHZ_ALUTECH_AT_4N_DIR_NAME);
    subghz_environment_set_nice_flor_s_rainbow_table_file_name(
        environment, SUBGHZ_NICE_FLOR_S_DIR_NAME);
    subghz_environment_set_protocol_registry(environment, (void*)&subghz_protocol_registry);
    return environment;
}

static void send_keeloq(
    uint32_t frequency,
    uint32_t serial,
    uint8_t btn,
    uint16_t cnt,
    const char* name_sysmem) {
    if(!furi_hal_region_is_frequency_allowed(frequency)) {
        // TODO: Show friendly UI message if frequency is not allowed.
        FURI_LOG_E(TAG, "Frequency %lu is not allowed in this region.", frequency);
        return;
    }

    FURI_LOG_I(TAG, "Sending signal on frequency %lu", frequency);

    // Populate the CC101 device list.
    subghz_devices_init();

    // Get the internal radio device.
    const SubGhzDevice* device = subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_INT_NAME);

    // Get the Princeton SubGhzTransmitter (for decoding our file format).
    SubGhzEnvironment* environment = load_environment();
    subghz_environment_set_protocol_registry(environment, (void*)&subghz_protocol_registry);
    SubGhzTransmitter* transmitter =
        subghz_transmitter_alloc_init(environment, SUBGHZ_PROTOCOL_KEELOQ_NAME);

    // Load the payload we want to send into flipper_format.
    FlipperFormat* flipper_format = flipper_format_string_alloc();

    SubGhzRadioPreset* preset = malloc(sizeof(SubGhzRadioPreset));
    preset->frequency = frequency;
    preset->name = furi_string_alloc();
    furi_string_set(preset->name, "AM650");
    preset->data = NULL;
    preset->data_size = 0;

    SubGhzProtocolEncoderBase* encoder = subghz_transmitter_get_protocol_instance(transmitter);
    // sadly, in some firmware this has a Repeat of 100, which is too much for our purposes.

    subghz_protocol_keeloq_create_data(
        encoder, flipper_format, serial, btn, cnt, name_sysmem, preset);

    // Fill out the SubGhzProtocolDecoderPrinceton (which includes SubGhzBlockGeneric data) in our transmitter based on parsing flipper_format.
    // initance->encoder.upload[] gets filled out with duration and level information (You can think of this as the RAW data).
    SubGhzProtocolStatus status = subghz_transmitter_deserialize(transmitter, flipper_format);
    furi_assert(status == SubGhzProtocolStatusOk);

    // Currently unused for internal radio, but good idea to still invoke it.
    subghz_devices_begin(device);

    // Initializes the CC1101 SPI bus
    subghz_devices_reset(device);

    // Use one of the presets in subghz_device_cc1101_int_interconnect_load_preset.  If the first argument is FuriHalSubGhzPresetCustom, then the second argument is
    // a custom register table (Reg, value, reg, value, ...,0, 0, PATable [0..7] entries).
    subghz_devices_load_preset(device, FuriHalSubGhzPresetOok650Async, NULL);

    // Set the frequency, RF switch path (band), calibrates the oscillator on the CC1101.
    frequency = subghz_devices_set_frequency(device, frequency);

    // Stop charging the battery while transmitting.
    furi_hal_power_suppress_charge_enter();

    // Start transmitting (keeps the DMA buffer filled with the encoder.upload[] data)
    if(subghz_devices_start_async_tx(device, subghz_transmitter_yield, transmitter)) {
        int max_counter = 10;

        // Wait for the transmission to complete, or counter to expire (about 1 second).
        while(max_counter-- && !(subghz_devices_is_async_complete_tx(device))) {
            furi_delay_ms(100);
        }

        // Stop transmitting, debug log (tag="FuriHalSubGhz") the duty cycle information.
        subghz_devices_stop_async_tx(device);
    }

    // clean up and shutdown cc1101
    subghz_devices_sleep(device);

    // also does a shutdown of cc1101
    subghz_devices_end(device);

    // remove the devices from the registry
    subghz_devices_deinit();

    // Allow the battery to charge again.
    furi_hal_power_suppress_charge_exit();

    // Free resources we allocated.
    flipper_format_free(flipper_format);
    subghz_transmitter_free(transmitter);
    subghz_environment_free(environment);
}

void send_keeloq_count(uint32_t fix, uint32_t count, const char* name, uint32_t frequency) {
    uint32_t serial = fix & 0x0FFFFFFF;
    uint8_t btn = fix >> 28;
    send_keeloq(frequency, serial, btn, count, name);
}