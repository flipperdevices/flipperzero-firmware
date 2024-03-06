// Methods for Sub-GHz transmission

// subghz
#include <lib/subghz/transmitter.h>
#include <lib/subghz/devices/devices.h>
#include <lib/subghz/devices/cc1101_configs.h>
#include <lib/subghz/protocols/raw.h>
#include <lib/subghz/subghz_protocol_registry.h>

#include "action_i.h"

static FuriHalSubGhzPreset action_subghz_get_preset_name(const char* preset_name) {
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
    } else {
        FURI_LOG_E(TAG, "Unknown preset!");
    }
    return preset;
}

// Lifted from flipperzero-firmware/applications/main/subghz/subghz_cli.c
void action_subghz_tx(void* context, Item* item) {
    App* app = context;
    FuriString* file_name = item->path;
    uint32_t repeat = 1; // 10?
    // uint32_t device_ind = 0; // 0 - CC1101_INT, 1 - CC1101_EXT

    FlipperFormat* fff_data_file = flipper_format_file_alloc(app->storage);
    FlipperFormat* fff_data_raw = flipper_format_string_alloc();
    FuriString* temp_str;
    temp_str = furi_string_alloc();
    uint32_t temp_data32;
    bool check_file = false;
    const SubGhzDevice* device = NULL;

    uint32_t frequency = 0;
    SubGhzTransmitter* transmitter = NULL;

    FURI_LOG_I(TAG, "action_run_tx starting...");

    subghz_devices_init();
    SubGhzEnvironment* environment = subghz_environment_alloc();
    if(!subghz_environment_load_keystore(environment, SUBGHZ_KEYSTORE_DIR_NAME)) {
        FURI_LOG_E(TAG, "Load_keystore keeloq_mfcodes ERROR");
    }
    if(!subghz_environment_load_keystore(environment, SUBGHZ_KEYSTORE_DIR_USER_NAME)) {
        FURI_LOG_E(TAG, "Load_keystore keeloq_mfcodes_user ERROR");
    }
    subghz_environment_set_came_atomo_rainbow_table_file_name(
        environment, SUBGHZ_CAME_ATOMO_DIR_NAME);
    subghz_environment_set_alutech_at_4n_rainbow_table_file_name(
        environment, SUBGHZ_ALUTECH_AT_4N_DIR_NAME);
    subghz_environment_set_nice_flor_s_rainbow_table_file_name(
        environment, SUBGHZ_NICE_FLOR_S_DIR_NAME);
    subghz_environment_set_protocol_registry(environment, (void*)&subghz_protocol_registry);

    do {
        // SUBGHZ_DEVICE_CC1101_INT_NAME = "cc1101_int"
        device = subghz_devices_get_by_name("cc1101_int");
        if(!subghz_devices_is_connect(device)) {
            // power off
            if(furi_hal_power_is_otg_enabled()) furi_hal_power_disable_otg();
            device = subghz_devices_get_by_name("cc1101_int");
            // device_ind = 0;
        }

        if(!flipper_format_file_open_existing(fff_data_file, furi_string_get_cstr(file_name))) {
            FURI_LOG_E(TAG, "Error opening %s", furi_string_get_cstr(file_name));
            break;
        }

        if(!flipper_format_read_header(fff_data_file, temp_str, &temp_data32)) {
            FURI_LOG_E(TAG, "Missing or incorrect header");
            break;
        }

        if(((!strcmp(furi_string_get_cstr(temp_str), SUBGHZ_KEY_FILE_TYPE)) ||
            (!strcmp(furi_string_get_cstr(temp_str), SUBGHZ_RAW_FILE_TYPE))) &&
           temp_data32 == SUBGHZ_KEY_FILE_VERSION) {
        } else {
            FURI_LOG_E(TAG, "Type or version mismatch");
            break;
        }

        if(!flipper_format_read_uint32(fff_data_file, "Frequency", &frequency, 1)) {
            FURI_LOG_E(TAG, "Missing Frequency");
            break;
        }

        if(!subghz_devices_is_frequency_valid(device, frequency)) {
            FURI_LOG_E(TAG, "Frequency not supported");
            break;
        }

        if(!flipper_format_read_string(fff_data_file, "Preset", temp_str)) {
            FURI_LOG_E(TAG, "Missing Preset");
            break;
        }

        subghz_devices_begin(device);
        subghz_devices_reset(device);

        if(!strcmp(furi_string_get_cstr(temp_str), "FuriHalSubGhzPresetCustom")) {
            uint8_t* custom_preset_data;
            uint32_t custom_preset_data_size;
            if(!flipper_format_get_value_count(fff_data_file, "Custom_preset_data", &temp_data32))
                break;
            if(!temp_data32 || (temp_data32 % 2)) {
                FURI_LOG_E(TAG, "Custom_preset_data size error");
                break;
            }
            custom_preset_data_size = sizeof(uint8_t) * temp_data32;
            custom_preset_data = malloc(custom_preset_data_size);
            if(!flipper_format_read_hex(
                   fff_data_file,
                   "Custom_preset_data",
                   custom_preset_data,
                   custom_preset_data_size)) {
                FURI_LOG_E(TAG, "Custom_preset_data read error");
                break;
            }
            subghz_devices_load_preset(
                device,
                action_subghz_get_preset_name(furi_string_get_cstr(temp_str)),
                custom_preset_data);
            free(custom_preset_data);
        } else {
            subghz_devices_load_preset(
                device, action_subghz_get_preset_name(furi_string_get_cstr(temp_str)), NULL);
        }

        subghz_devices_set_frequency(device, frequency);

        // Load Protocol
        if(!flipper_format_read_string(fff_data_file, "Protocol", temp_str)) {
            FURI_LOG_E(TAG, "Missing protocol");
            break;
        }

        SubGhzProtocolStatus status;
        bool is_init_protocol = true;
        if(!strcmp(furi_string_get_cstr(temp_str), "RAW")) {
            FURI_LOG_I(TAG, "Protocol = RAW");
            subghz_protocol_raw_gen_fff_data(
                fff_data_raw, furi_string_get_cstr(file_name), subghz_devices_get_name(device));
            transmitter =
                subghz_transmitter_alloc_init(environment, furi_string_get_cstr(temp_str));
            if(transmitter == NULL) {
                FURI_LOG_E(TAG, "Error transmitter");
                is_init_protocol = false;
            }

            if(is_init_protocol) {
                status = subghz_transmitter_deserialize(transmitter, fff_data_raw);
                if(status != SubGhzProtocolStatusOk) {
                    FURI_LOG_E(TAG, "Error deserialize protocol");
                    is_init_protocol = false;
                }
            }
        } else { // if not RAW protocol
            FURI_LOG_I(TAG, "Protocol != RAW");
            flipper_format_insert_or_update_uint32(fff_data_file, "Repeat", &repeat, 1);
            transmitter =
                subghz_transmitter_alloc_init(environment, furi_string_get_cstr(temp_str));
            if(transmitter == NULL) {
                FURI_LOG_E(TAG, "Error transmitter");
                is_init_protocol = false;
            }
            if(is_init_protocol) {
                status = subghz_transmitter_deserialize(transmitter, fff_data_file);
                if(status != SubGhzProtocolStatusOk) {
                    FURI_LOG_E(TAG, "Error deserialize protocol");
                    is_init_protocol = false;
                }
            }
            flipper_format_delete_key(fff_data_file, "Repeat");
        }

        if(is_init_protocol) {
            check_file = true;
        } else {
            subghz_devices_sleep(device);
            subghz_devices_end(device);
            subghz_transmitter_free(transmitter);
        }
    } while(false);

    flipper_format_free(fff_data_file);

    if(check_file) {
        furi_hal_power_suppress_charge_enter();
        FURI_LOG_I(
            TAG,
            "Listening at %s. Frequency=%lu, Protocol=%s",
            furi_string_get_cstr(file_name),
            frequency,
            furi_string_get_cstr(temp_str));
        do {
            // delay in downloading files and other preparatory processes
            furi_delay_ms(200);
            if(subghz_devices_start_async_tx(device, subghz_transmitter_yield, transmitter)) {
                while(!(subghz_devices_is_async_complete_tx(
                    device))) { // || cli_cmd_interrupt_received
                    furi_delay_ms(333);
                }
                subghz_devices_stop_async_tx(device);
            } else {
                FURI_LOG_W(TAG, "Transmission on this frequency is restricted in your region");
            }

            if(!strcmp(furi_string_get_cstr(temp_str), "RAW")) {
                subghz_transmitter_stop(transmitter);
                repeat--;
                if(repeat) subghz_transmitter_deserialize(transmitter, fff_data_raw);
            }

        } while(repeat && !strcmp(furi_string_get_cstr(temp_str), "RAW"));

        subghz_devices_sleep(device);
        subghz_devices_end(device);
        // power off
        if(furi_hal_power_is_otg_enabled()) furi_hal_power_disable_otg();

        furi_hal_power_suppress_charge_exit();

        subghz_transmitter_free(transmitter);
    }

    flipper_format_free(fff_data_raw);
    furi_string_free(temp_str);
    subghz_devices_deinit();
    subghz_environment_free(environment);
}
