#include <furi.h>
#include <lib/subghz/transmitter.h>
#include <flipper_format/flipper_format_i.h>
#include <lib/subghz/protocols/protocol_items.h>
#include "subghz_signal.h"
#include "app_config.h"
#include <storage/storage.h>

#ifdef FIRMWARE_SUPPORTS_SUBGHZ
#include <applications/drivers/subghz/cc1101_ext/cc1101_ext_interconnect.h>
#include <lib/subghz/devices/cc1101_int/cc1101_int_interconnect.h>
#include <devices/devices.h>

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

SubGhzSignal* subghz_signal_load_file(char* file_path) {
    SubGhzSignal* signal = (SubGhzSignal*)malloc(sizeof(SubGhzSignal));
    signal->sub_file_contents = furi_string_alloc();
    signal->protocol = furi_string_alloc();
    signal->frequency = 0;
    signal->preset = FuriHalSubGhzPresetOok650Async;

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
        FURI_LOG_E(TAG, "Error open file %s", file_path);
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
        furi_string_free(signal->protocol);
        free(signal);
        signal = NULL;
    }

    furi_record_close(RECORD_STORAGE);
    return signal;
}

void subghz_signal_free(SubGhzSignal* signal) {
    if(signal) {
        furi_string_free(signal->sub_file_contents);
        furi_string_free(signal->protocol);
        free(signal);
    }
}

void subghz_signal_send(SubGhzSignal* signal, bool use_external_radio) {
    if(signal) {
        send_signal(
            signal->sub_file_contents,
            furi_string_get_cstr(signal->protocol),
            signal->frequency,
            signal->preset,
            use_external_radio);
    }
}

/*

Future file browser example:

    FuriString* file_path = furi_string_alloc();

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(
        &browser_options, SUBGHZ_APP_FILENAME_EXTENSION, &I_sub1_10px);
    browser_options.base_path = SUBGHZ_APP_FOLDER;

    // Input events and views are managed by file_select
    bool res = dialog_file_browser_show(
        subghz->dialogs, subghz->file_path, subghz->file_path, &browser_options);

*/
