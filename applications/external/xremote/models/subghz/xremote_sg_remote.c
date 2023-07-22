#include "subghz/types.h"
#include <lib/toolbox/path.h>
#include <lib/subghz/types.h>
#include <lib/subghz/subghz_worker.h>
#include <lib/subghz/subghz_setting.h>
#include <lib/subghz/receiver.h>
#include <lib/subghz/transmitter.h>
#include <lib/subghz/registry.h>
//#include <lib/subghz/protocols/keeloq.h>
//#include <lib/subghz/protocols/secplus_v1.h>
//#include <lib/subghz/protocols/secplus_v2.h>
//#include <lib/subghz/protocols/princeton_for_testing.h>
//#include <lib/subghz/blocks/math.h>
//#include <lib/subghz/protocols/raw.h>
//#include <lib/subghz/protocols/bin_raw.h>
//#include <lib/subghz/protocols/protocol_items.h> //Not found
//#include <lib/subghz/protocols/protocol_items.c>
#include <lib/subghz/subghz_keystore.h>
//#include <lib/subghz/subghz_file_encoder_worker.h>
#include <gui/modules/variable_item_list.h>
#include "xremote_sg_remote.h"

#define TAG "Xremote"

struct SubGhzRemote {
    FuriString* name;
    FuriString* path;
    SubGhzTxRx* txrx;
    uint32_t frequency;
    SubGhzSetting* setting;
};

const char* xremote_sg_remote_get_name(SubGhzRemote* remote) {
    return furi_string_get_cstr(remote->name);
}

void subghz_preset_init(
    void* context,
    const char* preset_name,
    uint32_t frequency,
    uint8_t* preset_data,
    size_t preset_data_size) {
    furi_assert(context);
    SubGhzRemote* remote = context;
    furi_string_set(remote->txrx->preset->name, preset_name);
    remote->txrx->preset->frequency = frequency;
    remote->txrx->preset->data = preset_data;
    remote->txrx->preset->data_size = preset_data_size;
}

const char* subghz_txrx_radio_device_get_name(SubGhzTxRx* instance) {
    furi_assert(instance);
    return subghz_devices_get_name(instance->radio_device);
}

SubGhzRemote* xremote_sg_remote_alloc() {
    SubGhzRemote* remote = malloc(sizeof(SubGhzRemote));
    remote->name = furi_string_alloc();
    remote->path = furi_string_alloc();

    // SubGhz Settings
    remote->setting = subghz_setting_alloc();
    subghz_setting_load(remote->setting, EXT_PATH("subghz/assets/setting_user.txt"));

    remote->txrx = malloc(sizeof(SubGhzTxRx));
    remote->txrx->preset = malloc(sizeof(SubGhzRadioPreset));
    remote->txrx->preset->name = furi_string_alloc();
    subghz_preset_init(
        remote, "AM650", subghz_setting_get_default_frequency(remote->setting), NULL, 0);
    remote->txrx->fff_data = flipper_format_string_alloc();
    remote->txrx->environment = subghz_environment_alloc();
    /*subghz_environment_set_came_atomo_rainbow_table_file_name(
        remote->txrx->environment, EXT_PATH("subghz/assets/came_atomo"));
    subghz_environment_set_alutech_at_4n_rainbow_table_file_name(
        remote->txrx->environment, EXT_PATH("subghz/assets/alutech_at_4n"));
    subghz_environment_set_nice_flor_s_rainbow_table_file_name(
        remote->txrx->environment, EXT_PATH("subghz/assets/nice_flor_s"));
    subghz_environment_set_protocol_registry(
        remote->txrx->environment, (void*)&subghz_protocol_registry);
    remote->txrx->receiver = subghz_receiver_alloc_init(remote->txrx->environment);*/
    //remote->txrx->worker = subghz_worker_alloc();

    return remote;
}

void xremote_sg_remote_free(SubGhzRemote* remote) {
    furi_string_free(remote->path);
    furi_string_free(remote->name);

    // TXRX
    subghz_receiver_free(remote->txrx->receiver);
    subghz_environment_free(remote->txrx->environment);
    flipper_format_free(remote->txrx->fff_data);
    furi_string_free(remote->txrx->preset->name);
    free(remote->txrx->preset);
    free(remote->txrx);

    free(remote);
}

bool xremtoe_sg_set_preset(SubGhzRemote* remote, const char* preset) {
    if(!strcmp(preset, "FuriHalSubGhzPresetOok270Async")) {
        furi_string_set(remote->txrx->preset->name, "AM270");
    } else if(!strcmp(preset, "FuriHalSubGhzPresetOok650Async")) {
        furi_string_set(remote->txrx->preset->name, "AM650");
    } else if(!strcmp(preset, "FuriHalSubGhzPreset2FSKDev238Async")) {
        furi_string_set(remote->txrx->preset->name, "FM238");
    } else if(!strcmp(preset, "FuriHalSubGhzPreset2FSKDev476Async")) {
        furi_string_set(remote->txrx->preset->name, "FM476");
    } else if(!strcmp(preset, "FuriHalSubGhzPresetCustom")) {
        furi_string_set(remote->txrx->preset->name, "CUSTOM");
    } else {
        FURI_LOG_E(TAG, "Unknown preset");
        return false;
    }
    return true;
}

uint32_t xremote_sg_remote_get_frequency(SubGhzRemote* remote) {
    return remote->txrx->preset->frequency;
}

const char* xremote_sg_remote_get_preset(SubGhzRemote* remote) {
    return furi_string_get_cstr(remote->txrx->preset->name);
}

bool xremote_sg_remote_load(SubGhzRemote* remote, FuriString* path) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_buffered_file_alloc(storage);

    FuriString* buf;
    buf = furi_string_alloc();
    uint32_t temp_data32;

    FURI_LOG_I(TAG, "loading SG Remote: \'%s\'", furi_string_get_cstr(path));
    bool success = false;

    do {
        if(!flipper_format_buffered_file_open_existing(ff, furi_string_get_cstr(path))) break;
        const char* fullPath = furi_string_get_cstr(path);
        char* fileName = strrchr(fullPath, '/') + 1;
        char* dotPosition = strrchr(fileName, '.');
        if(dotPosition != NULL) { // check if there is a dot in the file name
            *dotPosition = '\0'; // set the dot position to NULL character to truncate the string
        }
        //remote->name = fileName;
        furi_string_set_str(remote->name, fileName);
        uint32_t version;
        if(!flipper_format_read_header(ff, buf, &version)) break;
        if(!furi_string_equal(buf, "Flipper SubGhz RAW File") || (version != 1)) break;

        if(!flipper_format_read_uint32(ff, "Frequency", &temp_data32, 1)) {
            FURI_LOG_E(TAG, "Missing Frequency");
            break;
        }
        remote->frequency = temp_data32;

        if(!flipper_format_read_string(ff, "Preset", buf)) {
            FURI_LOG_E(TAG, "Missing Preset");
            break;
        }
        if(!xremtoe_sg_set_preset(remote, furi_string_get_cstr(buf))) {
            break;
        }

        if(!strcmp(furi_string_get_cstr(buf), "FuriHalSubGhzPresetCustom")) {
            //Todo add Custom_preset_module
            //delete preset if it already exists
            subghz_setting_delete_custom_preset(
                remote->setting, furi_string_get_cstr(remote->txrx->preset->name));
            //load custom preset from file
            if(!subghz_setting_load_custom_preset(
                   remote->setting, furi_string_get_cstr(remote->txrx->preset->name), ff)) {
                FURI_LOG_E(TAG, "Missing Custom preset");
                break;
            }
        }

        if(!flipper_format_read_string(ff, "Protocol", buf)) {
            FURI_LOG_E(TAG, "Missing Protocol");
            break;
        }

        if(!strcmp(furi_string_get_cstr(buf), "RAW")) {
            subghz_protocol_raw_gen_fff_data(
                remote->txrx->fff_data,
                furi_string_get_cstr(path),
                subghz_txrx_radio_device_get_name(remote->txrx));
        } else {
            stream_copy_full(
                flipper_format_get_raw_stream(ff),
                flipper_format_get_raw_stream(remote->txrx->fff_data));
        }

        /*remote->txrx->decoder_result = subghz_receiver_search_decoder_base_by_name(
            remote->txrx->receiver, furi_string_get_cstr(buf));*/
        if(remote->txrx->decoder_result) {
            SubGhzProtocolStatus status = subghz_protocol_decoder_base_deserialize(
                remote->txrx->decoder_result, remote->txrx->fff_data);
            if(status != SubGhzProtocolStatusOk) {
                //load_key_state = SubGhzLoadKeyStateProtocolDescriptionErr;
                success = false;
                break;
            }
        } else {
            FURI_LOG_E(TAG, "Protocol not found");
            break;
        }

        success = true;
    } while(false);

    furi_string_free(buf);
    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);
    return success;
}