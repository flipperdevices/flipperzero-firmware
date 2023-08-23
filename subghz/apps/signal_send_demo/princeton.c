#include "./princeton.h"
#include "./tag.h"

#include <lib/subghz/transmitter.h>
#include <lib/subghz/devices/cc1101_int/cc1101_int_interconnect.h>
//#include <applications/drivers/subghz/cc1101_ext/cc1101_ext_interconnect.h>
#include <lib/subghz/protocols/protocol_items.h>
#include <lib/subghz/devices/devices.h>

static void
    set_princeton(FlipperFormat* flipper_format, uint32_t key, uint32_t te, uint32_t repeat) {
    uint32_t bits = 24;
    uint8_t data[8] = {0};
    data[5] = (uint8_t)((key >> 16) & 0xFFU);
    data[6] = (uint8_t)((key >> 8) & 0xFFU);
    data[7] = (uint8_t)(key & 0xFFU);

    flipper_format_insert_or_update_string_cstr(flipper_format, "Protocol", "Princeton");
    flipper_format_insert_or_update_uint32(flipper_format, "Bit", &bits, 1);
    flipper_format_insert_or_update_hex(flipper_format, "Key", data, COUNT_OF(data));
    flipper_format_insert_or_update_uint32(flipper_format, "TE", &te, 1);
    flipper_format_insert_or_update_uint32(flipper_format, "Repeat", &repeat, 1);
    flipper_format_rewind(flipper_format);
}

void send_princeton(uint32_t key, uint32_t frequency) {
    // 'TE' is the microsecond (us) duration of the smallest pulse (effectively a bit in the BIN_RAW signal).
    // NOTE: If we wanted the Princeton signal to NOT be detected by FZ we could add +300+50, but receiver might still work (values from te_short+te_delta+50 from princeton.c)
    uint32_t te = 390;

    // 'repeat' is the number of times to repeat the signal.
    uint32_t repeat = 5;

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
    SubGhzEnvironment* environment = subghz_environment_alloc();
    subghz_environment_set_protocol_registry(environment, (void*)&subghz_protocol_registry);
    SubGhzTransmitter* transmitter = subghz_transmitter_alloc_init(environment, "Princeton");

    // Load the payload we want to send into flipper_format.
    FlipperFormat* flipper_format = flipper_format_string_alloc();
    set_princeton(flipper_format, key, te, repeat);

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
        // Wait for the transmission to complete.
        while(!(subghz_devices_is_async_complete_tx(device))) {
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
