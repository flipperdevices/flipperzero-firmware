// Methods for Sub-GHz transmission

// subghz
#include <lib/subghz/transmitter.h>
#include <lib/subghz/devices/devices.h>
#include <lib/subghz/devices/cc1101_configs.h>
#include <lib/subghz/protocols/raw.h>
#include <lib/subghz/subghz_protocol_registry.h>

#include <flipper_format/flipper_format.h>

#include "action_i.h"
#include "quac.h"

#define SUBGHZ_DEVICE_CC1101_EXT_NAME "cc1101_ext"
#define SUBGHZ_DEVICE_CC1101_INT_NAME "cc1101_int"

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
        FURI_LOG_E(TAG, "SUBGHZ: Unknown preset!");
    }
    return preset;
}

static const SubGhzDevice* action_subghz_get_device(uint32_t* device_ind) {
    const SubGhzDevice* device = NULL;
    switch(*device_ind) {
    case 1: {
        // Power on the external antenna
        uint8_t attempts = 5;
        while(--attempts > 0) {
            if(furi_hal_power_enable_otg()) break;
        }
        if(attempts == 0) {
            if(furi_hal_power_get_usb_voltage() < 4.5f) {
                FURI_LOG_E(
                    TAG,
                    "Error power otg enable. BQ2589 check otg fault = %d",
                    furi_hal_power_check_otg_fault() ? 1 : 0);
            }
        }
        device = subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_EXT_NAME);
        break;
    }
    default:
        device = subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_INT_NAME);
        break;
    }
    if(!subghz_devices_is_connect(device)) {
        // Power off
        if(furi_hal_power_is_otg_enabled()) {
            furi_hal_power_disable_otg();
        }
        if(*device_ind == 1) {
            FURI_LOG_W(TAG, "Can't connect to External antenna, using Internal");
        }
        device = subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_INT_NAME);
        *device_ind = 0;
    }
    return device;
}

// Lifted from flipperzero-firmware/applications/main/subghz/subghz_cli.c
void action_subghz_tx(void* context, const FuriString* action_path, FuriString* error) {
    App* app = context;
    const char* file_name = furi_string_get_cstr(action_path);
    uint32_t repeat = app->settings.subghz_repeat; // Defaults to 10 in the CLI
    uint32_t device_ind = app->settings.subghz_use_ext_antenna ? 1 : 0;

    FlipperFormat* fff_data_file = flipper_format_file_alloc(app->storage);
    FlipperFormat* fff_data_raw = flipper_format_string_alloc();
    FuriString* temp_str;
    temp_str = furi_string_alloc();
    uint32_t temp_data32;
    bool check_file = false;
    const SubGhzDevice* device = NULL;

    uint32_t frequency = 0;
    SubGhzTransmitter* transmitter = NULL;

    FURI_LOG_I(TAG, "SUBGHZ: Action starting...");

    subghz_devices_init();
    SubGhzEnvironment* environment = subghz_environment_alloc();
    if(!subghz_environment_load_keystore(environment, SUBGHZ_KEYSTORE_DIR_NAME)) {
        FURI_LOG_W(TAG, "Load_keystore keeloq_mfcodes - failed to load");
    }
    if(!subghz_environment_load_keystore(environment, SUBGHZ_KEYSTORE_DIR_USER_NAME)) {
        FURI_LOG_W(TAG, "Load_keystore keeloq_mfcodes_user - failed to load");
    }
    subghz_environment_set_came_atomo_rainbow_table_file_name(
        environment, SUBGHZ_CAME_ATOMO_DIR_NAME);
    subghz_environment_set_alutech_at_4n_rainbow_table_file_name(
        environment, SUBGHZ_ALUTECH_AT_4N_DIR_NAME);
    subghz_environment_set_nice_flor_s_rainbow_table_file_name(
        environment, SUBGHZ_NICE_FLOR_S_DIR_NAME);
    subghz_environment_set_protocol_registry(environment, (void*)&subghz_protocol_registry);

    do {
        device = action_subghz_get_device(&device_ind);
        if(device == NULL) {
            FURI_LOG_E(TAG, "Error device not found");
            ACTION_SET_ERROR("SUBGHZ: Device not found");
            break;
        }

        if(!flipper_format_file_open_existing(fff_data_file, file_name)) {
            FURI_LOG_E(TAG, "Error opening %s", file_name);
            ACTION_SET_ERROR("SUBGHZ: Error opening %s", file_name);
            break;
        }

        if(!flipper_format_read_header(fff_data_file, temp_str, &temp_data32)) {
            FURI_LOG_E(TAG, "Missing or incorrect header");
            ACTION_SET_ERROR("SUBGHZ: Missing or incorrect header");
            break;
        }

        if(((!strcmp(furi_string_get_cstr(temp_str), SUBGHZ_KEY_FILE_TYPE)) ||
            (!strcmp(furi_string_get_cstr(temp_str), SUBGHZ_RAW_FILE_TYPE))) &&
           temp_data32 == SUBGHZ_KEY_FILE_VERSION) {
        } else {
            FURI_LOG_E(TAG, "Type or version mismatch");
            ACTION_SET_ERROR("SUBGHZ: Type or version mismatch");
            break;
        }

        if(!flipper_format_read_uint32(fff_data_file, "Frequency", &frequency, 1)) {
            FURI_LOG_E(TAG, "Missing Frequency");
            ACTION_SET_ERROR("SUBGHZ: Missing frequency");
            break;
        }

        if(!subghz_devices_is_frequency_valid(device, frequency)) {
            FURI_LOG_E(TAG, "Frequency not supported");
            ACTION_SET_ERROR("SUBGHZ: Frequency not supported");
            break;
        }

        if(!flipper_format_read_string(fff_data_file, "Preset", temp_str)) {
            FURI_LOG_E(TAG, "Missing Preset");
            ACTION_SET_ERROR("SUBGHZ: Missing preset");
            break;
        }

        FuriHalSubGhzPreset preset = action_subghz_get_preset_name(furi_string_get_cstr(temp_str));
        if(preset == FuriHalSubGhzPresetIDLE) {
            ACTION_SET_ERROR("SUBGHZ: Unknown preset");
            break;
        }

        subghz_devices_begin(device);
        subghz_devices_reset(device);
        subghz_devices_idle(device);

        if(preset == FuriHalSubGhzPresetCustom) {
            uint8_t* custom_preset_data;
            uint32_t custom_preset_data_size;
            if(!flipper_format_get_value_count(fff_data_file, "Custom_preset_data", &temp_data32))
                break;
            if(!temp_data32 || (temp_data32 % 2)) {
                FURI_LOG_E(TAG, "Custom_preset_data size error");
                ACTION_SET_ERROR("SUBGHZ: Custom_preset_data size error");
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
                ACTION_SET_ERROR("SUBGHZ: Custom_preset_data read error");
                break;
            }
            subghz_devices_load_preset(device, preset, custom_preset_data);
            free(custom_preset_data);
        } else {
            subghz_devices_load_preset(device, preset, NULL);
        }

        subghz_devices_set_frequency(device, frequency);

        // Load Protocol
        if(!flipper_format_read_string(fff_data_file, "Protocol", temp_str)) {
            FURI_LOG_E(TAG, "Missing protocol");
            ACTION_SET_ERROR("SUBGHZ: Missing protocol");
            break;
        }

        SubGhzProtocolStatus status;
        bool is_init_protocol = true;
        if(furi_string_equal(temp_str, "RAW")) {
            FURI_LOG_I(TAG, "Protocol = RAW");
            subghz_protocol_raw_gen_fff_data(
                fff_data_raw, file_name, subghz_devices_get_name(device));
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
                    ACTION_SET_ERROR("SUBGHZ: Protocol error");
                    is_init_protocol = false;
                }
            }
        }

        if(is_init_protocol) {
            check_file = true;
        } else {
            subghz_devices_sleep(device);
            subghz_devices_end(device);
            if(transmitter != NULL) {
                subghz_transmitter_free(transmitter);
            }
        }
    } while(false);

    flipper_format_free(fff_data_file);

    if(check_file) {
        furi_hal_power_suppress_charge_enter();
        subghz_devices_set_tx(device);
        FURI_LOG_I(
            TAG,
            "Transmitting at %s. Frequency=%lu, Protocol=%s",
            file_name,
            frequency,
            furi_string_get_cstr(temp_str));
        do {
            // FURI_LOG_I(TAG, "delaying 200ms");
            furi_delay_ms(100); // needed? orig 200
            if(subghz_devices_start_async_tx(device, subghz_transmitter_yield, transmitter)) {
                while(!subghz_devices_is_async_complete_tx(device)) {
                    // || cli_cmd_interrupt_received
                    furi_delay_ms(100); // orig 333
                }
                subghz_devices_stop_async_tx(device);
            } else {
                FURI_LOG_W(TAG, "Transmission on this frequency is restricted in your region");
            }

            if(furi_string_equal(temp_str, "RAW")) {
                subghz_transmitter_stop(transmitter);
                repeat--;
                // FURI_LOG_I(TAG, "decrementing repeat: %lu", repeat);
                if(repeat) subghz_transmitter_deserialize(transmitter, fff_data_raw);
            }

        } while(repeat && furi_string_equal(temp_str, "RAW"));

        subghz_devices_sleep(device);
        subghz_devices_end(device);
        // power off
        if(furi_hal_power_is_otg_enabled()) furi_hal_power_disable_otg();

        furi_hal_power_suppress_charge_exit();
        subghz_transmitter_free(transmitter);
    }

    FURI_LOG_I(TAG, "SUBGHZ: Action complete.");

    flipper_format_free(fff_data_raw);
    furi_string_free(temp_str);
    subghz_devices_deinit();
    subghz_environment_free(environment);
}
