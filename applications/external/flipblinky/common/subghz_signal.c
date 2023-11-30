#include <furi.h>
#include <flipper_format/flipper_format_i.h>
#include <storage/storage.h>
#include <lib/subghz/protocols/protocol_items.h>
#include <lib/subghz/transmitter.h>

#include "../app_config.h"
#include "subghz_signal.h"

#ifdef FIRMWARE_SUPPORTS_SUBGHZ
#include <applications/drivers/subghz/cc1101_ext/cc1101_ext_interconnect.h>
#include <devices/devices.h>
#include <lib/subghz/devices/cc1101_int/cc1101_int_interconnect.h>

struct SubGhzSignal {
    FuriString* sub_file_contents;
    FuriString* protocol;
    uint32_t frequency;
    FuriHalSubGhzPreset preset;
};

typedef enum {
    Unknown,
    Raw,
    Key,
} FileType;

/**
 * @brief Turns on the +5V rail to the external radio.
*/
static void subghz_radio_device_power_on() {
    uint8_t attempts = 5;
    while(--attempts > 0) {
        if(furi_hal_power_enable_otg()) break;
        furi_delay_ms(50);
    }
}

/**
 * @brief Turns off the +5V rail to the external radio.
*/
static void subghz_radio_device_power_off() {
    if(furi_hal_power_is_otg_enabled()) furi_hal_power_disable_otg();
}

/**
 * @brief Populates a flipper_format with the contents of a FuriString.
 * @param flipper_format The flipper_format to populate.
 * @param contents The FuriString to populate the flipper_format with.
*/
static void __flipper_format_populate(FlipperFormat* flipper_format, FuriString* contents) {
    Stream* stream = flipper_format_get_raw_stream(flipper_format);
    stream_clean(stream);
    stream_write_cstring(stream, furi_string_get_cstr(contents));
    stream_seek(stream, 0, StreamOffsetFromStart);
}

/**
 * @brief Sends a signal using the SubGhz module.
 * @param sub_file_contents The contents of the signal file (can be RAW or Protocol).
 * @param protocol The protocol to use.
 * @param frequency The frequency to use.
 * @param preset The preset to use.
 * @param use_external_radio Currently only the internal radio is supported (please use 'false').  Whether or not to use the external radio.  
*/
static void send_signal(
    FuriString* sub_file_contents,
    const char* protocol,
    uint32_t frequency,
    FuriHalSubGhzPreset preset,
    bool use_external_radio) {
    bool enable_5v = use_external_radio;
    const SubGhzDevice* device;

    subghz_devices_init();
    if(use_external_radio) {
        if(enable_5v) {
            subghz_radio_device_power_on();
        }
        // TODO: I haven't tested EXT, so not sure if there is additional setup required.
        device = subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_EXT_NAME);
    } else {
        device = subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_INT_NAME);
    }

    SubGhzEnvironment* environment = subghz_environment_alloc();
    subghz_environment_set_protocol_registry(environment, (void*)&subghz_protocol_registry);
    SubGhzTransmitter* transmitter = subghz_transmitter_alloc_init(environment, protocol);
    FlipperFormat* flipper_format = flipper_format_string_alloc();
    __flipper_format_populate(flipper_format, sub_file_contents);
    subghz_transmitter_deserialize(transmitter, flipper_format);
    subghz_devices_begin(device);
    subghz_devices_reset(device);
    // TODO: I haven't testing Custom preset, so not sure if there is additional setup required.
    subghz_devices_load_preset(device, preset, NULL);
    frequency = subghz_devices_set_frequency(device, frequency);

    // subghz_devices_set_async_mirror_pin -- blink the board status LED with the signal data?

    // Send
    furi_hal_power_suppress_charge_enter();
    if(subghz_devices_start_async_tx(device, subghz_transmitter_yield, transmitter)) {
        // TODO: Allow cancelling the transmission when button is no longer pressed.
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
    FuriString* sub_file_contents,
    const char* protocol,
    uint32_t frequency,
    FuriHalSubGhzPreset preset,
    bool use_external_radio) {
    FURI_LOG_D("FlipSignal", "TODO: Send %s signal", protocol);
    UNUSED(sub_file_contents);
    UNUSED(frequency);
    UNUSED(preset);
    UNUSED(use_external_radio);
}
#endif

/**
 * @brief Converts a string to a FuriHalSubGhzPreset.
 * @param preset_str The string to convert.
 * @return The FuriHalSubGhzPreset.
*/
static FuriHalSubGhzPreset signal_get_preset(FuriString* preset_str) {
    const char* preset_name = furi_string_get_cstr(preset_str);
    FuriHalSubGhzPreset preset = FuriHalSubGhzPresetIDLE;
    if(!strcmp(preset_name, "FuriHalSubGhzPresetOok270Async")) {
        preset = FuriHalSubGhzPresetOok270Async;
    } else if(!strcmp(preset_name, "FuriHalSubGhzPresetOok650Async")) {
        preset = FuriHalSubGhzPresetOok650Async;
    } else if(!strcmp(preset_name, "FuriHalSubGhzPreset2FSKDev238Async")) {
        preset = FuriHalSubGhzPreset2FSKDev238Async;
    } else if(!strcmp(preset_name, "FuriHalSubGhzPreset2FSKDev476Async")) {
        preset = FuriHalSubGhzPreset2FSKDev476Async;
    } else if(!strcmp(preset_name, "FuriHalSubGhzPresetCustom")) {
        preset = FuriHalSubGhzPresetCustom;
    } else if(!strcmp(preset_name, "FuriHalSubGhzPresetMSK99_97KbAsync")) {
        preset = FuriHalSubGhzPresetMSK99_97KbAsync;
    } else if(!strcmp(preset_name, "FuriHalSubGhzPresetGFSK9_99KbAsync")) {
        preset = FuriHalSubGhzPresetGFSK9_99KbAsync;
    }
    return preset;
}

/**
 * @brief Loads a SubGhzSignal from a file (supports RAW and Protocol .SUB files)
 * @param file_path The path to the file to load.
 * @return The loaded SubGhzSignal. Be sure to call subghz_signal_free.
*/
SubGhzSignal* subghz_signal_load_file(char* file_path) {
    SubGhzSignal* signal = (SubGhzSignal*)malloc(sizeof(SubGhzSignal));
    signal->sub_file_contents = furi_string_alloc();
    signal->protocol = furi_string_alloc();
    signal->frequency = 0;
    signal->preset = FuriHalSubGhzPresetIDLE;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* fff_data_file = flipper_format_file_alloc(storage);
    FuriString* temp_str = furi_string_alloc();
    uint32_t temp_data32;
    FileType file_type = Unknown;
    bool parsed = false;

    if(flipper_format_file_open_existing(fff_data_file, file_path)) {
        do {
            if(!flipper_format_read_header(fff_data_file, temp_str, &temp_data32)) {
                FURI_LOG_E(TAG, "Error reading header");
                break;
            }

            if(strcmp(furi_string_get_cstr(temp_str), SUBGHZ_KEY_FILE_TYPE) == 0) {
                file_type = Key;
            } else if(strcmp(furi_string_get_cstr(temp_str), SUBGHZ_RAW_FILE_TYPE) == 0) {
                file_type = Raw;
            } else {
                FURI_LOG_E(TAG, "Unknown file type: %s", furi_string_get_cstr(temp_str));
                break;
            }

            if(!flipper_format_read_uint32(fff_data_file, "Frequency", &temp_data32, 1)) {
                FURI_LOG_E(TAG, "Missing Frequency");
                break;
            }
            signal->frequency = temp_data32;

            if(!flipper_format_read_string(fff_data_file, "Preset", temp_str)) {
                FURI_LOG_E(TAG, "Missing Preset");
                break;
            }
            signal->preset = signal_get_preset(temp_str);
            if(!signal->preset) {
                FURI_LOG_E(TAG, "Unknown preset");
                break;
            }

            if(!flipper_format_read_string(fff_data_file, "Protocol", temp_str)) {
                FURI_LOG_E(TAG, "Missing Protocol");
                break;
            }
            furi_string_set(signal->protocol, temp_str);

            parsed = true;
        } while(false);
    } else {
        FURI_LOG_E(TAG, "Error opening file %s", file_path);
    }

    furi_string_free(temp_str);
    flipper_format_free(fff_data_file);

    if(parsed && file_type == Raw) {
        furi_string_set(signal->sub_file_contents, temp_str);
        furi_string_printf(
            signal->sub_file_contents,
            "Filetype: RAW File Load\r\n"
            "Version: 1\r\n"
            "Protocol: RAW\r\n"
            "File_name: %s\r\n"
            // TODO: I haven't tested EXT, so not sure if there is additional setup required.
            "Radio_device_name: cc1101_int\r\n",
            file_path);
    } else if(parsed && file_type == Key) {
        File* file = storage_file_alloc(storage);
        if(storage_file_open(file, file_path, FSAM_READ, FSOM_OPEN_EXISTING)) {
            uint64_t file_size = storage_file_size(file);
            char* file_contents = (char*)malloc(file_size + 1);
            if(storage_file_read(file, file_contents, file_size) == file_size) {
                file_contents[file_size] = '\0';
                furi_string_set(signal->sub_file_contents, file_contents);
            } else {
                FURI_LOG_E(TAG, "Error reading file %s", file_path);
                parsed = false;
            }
            free(file_contents);
            storage_file_close(file);
        } else {
            FURI_LOG_E(TAG, "Error open file %s", file_path);
            parsed = false;
        }
    }

    if(!parsed) {
        subghz_signal_free(signal);
        signal = NULL;
    }

    furi_record_close(RECORD_STORAGE);
    return signal;
}

/**
 * @brief Frees a SubGhzSignal.
 * @param signal The SubGhzSignal to free.
*/
void subghz_signal_free(SubGhzSignal* signal) {
    if(signal) {
        if(signal->sub_file_contents) {
            furi_string_free(signal->sub_file_contents);
            signal->sub_file_contents = NULL;
        }
        if(signal->protocol) {
            furi_string_free(signal->protocol);
            signal->protocol = NULL;
        }
        free(signal);
    }
}

/**
 * @brief Sends a SubGhzSignal.
 * @param signal The SubGhzSignal to send.
 * @param use_external_radio Currently only the internal radio is supported (please use 'false').  Whether or not to use the external radio.
 * @param resources Resources used for sending the signal.
*/
void subghz_signal_send(SubGhzSignal* signal, bool use_external_radio, Resources* resources) {
    if(signal) {
        resources_acquire(resources, ResourceIdSubGhzSignal, FuriWaitForever);
        send_signal(
            signal->sub_file_contents,
            furi_string_get_cstr(signal->protocol),
            signal->frequency,
            signal->preset,
            use_external_radio);
        resources_release(resources, ResourceIdSubGhzSignal);
    }
}
