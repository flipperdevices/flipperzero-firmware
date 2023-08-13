#include "uhf_device.h"

#include <toolbox/path.h>
#include <flipper_format/flipper_format.h>
#include <uhf_rfid_icons.h>

#define TAG "UHFDevice"

static const char* uhf_file_header = "Flipper UHF RFID device";
static const uint32_t uhf_file_version = 1;
// static const uint8_t bank_data_start = 20;
// static const uint8_t bank_data_length = 16;

UHFDevice* uhf_device_alloc() {
    UHFDevice* uhf_device = malloc(sizeof(UHFDevice));
    uhf_device->storage = furi_record_open(RECORD_STORAGE);
    uhf_device->dialogs = furi_record_open(RECORD_DIALOGS);
    uhf_device->load_path = furi_string_alloc();
    return uhf_device;
}

void uhf_device_set_name(UHFDevice* dev, const char* name) {
    furi_assert(dev);

    strlcpy(dev->dev_name, name, UHF_DEV_NAME_MAX_LEN);
}

static bool uhf_device_save_file(
    UHFDevice* dev,
    const char* dev_name,
    const char* folder,
    const char* extension,
    bool use_load_path) {
    furi_assert(dev);

    UHFTag* uhf_tag = dev->uhf_tag;
    bool saved = false;
    FlipperFormat* file = flipper_format_file_alloc(dev->storage);
    FuriString* temp_str;
    temp_str = furi_string_alloc();
    do {
        if(use_load_path && !furi_string_empty(dev->load_path)) {
            // Get directory name
            path_extract_dirname(furi_string_get_cstr(dev->load_path), temp_str);
            // Make path to file to save
            furi_string_cat_printf(temp_str, "/%s%s", dev_name, extension);
        } else {
            // First remove uhf device file if it was saved
            furi_string_printf(temp_str, "%s/%s%s", folder, dev_name, extension);
        }
        // Open file
        if(!flipper_format_file_open_always(file, furi_string_get_cstr(temp_str))) break;

        // Write header
        if(!flipper_format_write_header_cstr(file, uhf_file_header, uhf_file_version)) break;

        // Reserved bank might be added
        // todo : maybe
        uint32_t temp_arr[1];
        // write epc
        temp_arr[0] = uhf_tag->epc_length;
        if(!flipper_format_write_uint32(file, UHF_EPC_BANK_LENGTH_LABEL, temp_arr, 1)) break;
        if(!flipper_format_write_hex(file, UHF_EPC_BANK_LABEL, uhf_tag->epc, uhf_tag->epc_length))
            break;
        // write tid
        temp_arr[0] = uhf_tag->tid_length;
        if(!flipper_format_write_uint32(file, UHF_TID_BANK_LENGTH_LABEL, temp_arr, 1)) break;
        if(!flipper_format_write_hex(file, UHF_TID_BANK_LABEL, uhf_tag->tid, uhf_tag->tid_length))
            break;
        // write user
        temp_arr[0] = uhf_tag->user_length;
        if(!flipper_format_write_uint32(file, UHF_USER_BANK_LENGTH_LABEL, temp_arr, 1)) break;
        if(!flipper_format_write_hex(
               file, UHF_USER_BANK_LABEL, uhf_tag->user, uhf_tag->user_length))
            break;
        saved = true;
    } while(0);

    if(!saved) {
        dialog_message_show_storage_error(dev->dialogs, "Can not save\nfile");
    }
    furi_string_free(temp_str);
    flipper_format_free(file);
    return saved;
}

bool uhf_device_save(UHFDevice* dev, const char* dev_name) {
    return uhf_device_save_file(
        dev, dev_name, STORAGE_APP_DATA_PATH_PREFIX, UHF_APP_EXTENSION, true);

    return false;
}
// uncomment

static bool uhf_device_load_data(UHFDevice* dev, FuriString* path, bool show_dialog) {
    bool parsed = false;
    FlipperFormat* file = flipper_format_file_alloc(dev->storage);
    // UHFResponseData* uhf_response_data = dev->dev_data;
    FuriString* temp_str;
    temp_str = furi_string_alloc();
    bool deprecated_version = false;
    UHFTag* uhf_tag = dev->uhf_tag;
    uhf_tag_reset(uhf_tag);
    uint32_t temp_arr[1];
    if(dev->loading_cb) {
        dev->loading_cb(dev->loading_cb_ctx, true);
    }

    do {
        if(!flipper_format_file_open_existing(file, furi_string_get_cstr(path))) break;

        // Read and verify file header
        uint32_t version = 0;
        if(!flipper_format_read_header(file, temp_str, &version)) break;
        if(furi_string_cmp_str(temp_str, uhf_file_header) || (version != uhf_file_version)) {
            deprecated_version = true;
            break;
        }
        // read epc
        if(!flipper_format_read_uint32(file, UHF_EPC_BANK_LENGTH_LABEL, temp_arr, 1)) break;
        uhf_tag->epc_length = temp_arr[0];
        if(!flipper_format_read_hex(file, UHF_EPC_BANK_LABEL, uhf_tag->epc, uhf_tag->epc_length))
            break;

        // read tid
        if(!flipper_format_read_uint32(file, UHF_TID_BANK_LENGTH_LABEL, temp_arr, 1)) break;
        uhf_tag->tid_length = temp_arr[0];
        if(!flipper_format_read_hex(file, UHF_TID_BANK_LABEL, uhf_tag->tid, uhf_tag->tid_length))
            break;

        // read user
        if(!flipper_format_read_uint32(file, UHF_USER_BANK_LENGTH_LABEL, temp_arr, 1)) break;
        uhf_tag->user_length = temp_arr[0];
        if(!flipper_format_read_hex(file, UHF_USER_BANK_LABEL, uhf_tag->user, uhf_tag->user_length))
            break;

        parsed = true;
    } while(false);

    if(dev->loading_cb) {
        dev->loading_cb(dev->loading_cb_ctx, false);
    }

    if((!parsed) && (show_dialog)) {
        if(deprecated_version) {
            dialog_message_show_storage_error(dev->dialogs, "File format deprecated");
        } else {
            dialog_message_show_storage_error(dev->dialogs, "Can not parse\nfile");
        }
    }

    furi_string_free(temp_str);
    flipper_format_free(file);

    return parsed;
}

// void picopass_device_clear(UHFDevice* dev) {
//     furi_assert(dev);

//     picopass_device_data_clear(&dev->dev_data);
//     memset(&dev->dev_data, 0, sizeof(dev->dev_data));
//     dev->format = PicopassDeviceSaveFormatHF;
//     furi_string_reset(dev->load_path);
// }

void uhf_device_free(UHFDevice* uhf_dev) {
    furi_assert(uhf_dev);
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_DIALOGS);
    furi_string_free(uhf_dev->load_path);
    free(uhf_dev);
}

bool uhf_file_select(UHFDevice* dev) {
    furi_assert(dev);

    FuriString* uhf_app_folder;
    uhf_app_folder = furi_string_alloc_set(STORAGE_APP_DATA_PATH_PREFIX);

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, UHF_APP_EXTENSION, &I_Nfc_10px);
    browser_options.base_path = STORAGE_APP_DATA_PATH_PREFIX;

    bool res =
        dialog_file_browser_show(dev->dialogs, dev->load_path, uhf_app_folder, &browser_options);

    furi_string_free(uhf_app_folder);
    if(res) {
        FuriString* filename;
        filename = furi_string_alloc();
        path_extract_filename(dev->load_path, filename, true);
        strncpy(dev->dev_name, furi_string_get_cstr(filename), UHF_DEV_NAME_MAX_LEN);
        res = uhf_device_load_data(dev, dev->load_path, true);
        if(res) {
            uhf_device_set_name(dev, dev->dev_name);
        }
        furi_string_free(filename);
    }

    return res;
}

// void uhf_device_data_clear(UHFDevice* dev_data) {
//     for(size_t i = 0; i < PICOPASS_MAX_APP_LIMIT; i++) {
//         memset(dev_data->AA1[i].data, 0, sizeof(dev_data->AA1[i].data));
//     }
//     dev_data->pacs.legacy = false;
//     dev_data->pacs.se_enabled = false;
//     dev_data->pacs.elite_kdf = false;
//     dev_data->pacs.pin_length = 0;
// }

bool uhf_device_delete(UHFDevice* dev, bool use_load_path) {
    furi_assert(dev);

    bool deleted = false;
    FuriString* file_path;
    file_path = furi_string_alloc();

    do {
        // Delete original file
        if(use_load_path && !furi_string_empty(dev->load_path)) {
            furi_string_set(file_path, dev->load_path);
        } else {
            furi_string_printf(file_path, APP_DATA_PATH("%s%s"), dev->dev_name, UHF_APP_EXTENSION);
        }
        if(!storage_simply_remove(dev->storage, furi_string_get_cstr(file_path))) break;
        deleted = true;
    } while(0);

    if(!deleted) {
        dialog_message_show_storage_error(dev->dialogs, "Can not remove file");
    }

    furi_string_free(file_path);
    return deleted;
}

void uhf_device_set_loading_callback(UHFDevice* dev, UHFLoadingCallback callback, void* context) {
    furi_assert(dev);

    dev->loading_cb = callback;
    dev->loading_cb_ctx = context;
}

// ReturnCode picopass_device_decrypt(uint8_t* enc_data, uint8_t* dec_data) {
//     uint8_t key[32] = {0};
//     memcpy(key, picopass_iclass_decryptionkey, sizeof(picopass_iclass_decryptionkey));
//     mbedtls_des3_context ctx;
//     mbedtls_des3_init(&ctx);
//     mbedtls_des3_set2key_dec(&ctx, key);
//     mbedtls_des3_crypt_ecb(&ctx, enc_data, dec_data);
//     mbedtls_des3_free(&ctx);
//     return ERR_NONE;
// }

// ReturnCode picopass_device_parse_credential(PicopassBlock* AA1, PicopassPacs* pacs) {
//     ReturnCode err;

//     pacs->biometrics = AA1[6].data[4];
//     pacs->pin_length = AA1[6].data[6] & 0x0F;
//     pacs->encryption = AA1[6].data[7];

//     if(pacs->encryption == PicopassDeviceEncryption3DES) {
//         FURI_LOG_D(TAG, "3DES Encrypted");
//         err = picopass_device_decrypt(AA1[7].data, pacs->credential);
//         if(err != ERR_NONE) {
//             FURI_LOG_E(TAG, "decrypt error %d", err);
//             return err;
//         }

//         err = picopass_device_decrypt(AA1[8].data, pacs->pin0);
//         if(err != ERR_NONE) {
//             FURI_LOG_E(TAG, "decrypt error %d", err);
//             return err;
//         }

//         err = picopass_device_decrypt(AA1[9].data, pacs->pin1);
//         if(err != ERR_NONE) {
//             FURI_LOG_E(TAG, "decrypt error %d", err);
//             return err;
//         }
//     } else if(pacs->encryption == PicopassDeviceEncryptionNone) {
//         FURI_LOG_D(TAG, "No Encryption");
//         memcpy(pacs->credential, AA1[7].data, PICOPASS_BLOCK_LEN);
//         memcpy(pacs->pin0, AA1[8].data, PICOPASS_BLOCK_LEN);
//         memcpy(pacs->pin1, AA1[9].data, PICOPASS_BLOCK_LEN);
//     } else if(pacs->encryption == PicopassDeviceEncryptionDES) {
//         FURI_LOG_D(TAG, "DES Encrypted");
//     } else {
//         FURI_LOG_D(TAG, "Unknown encryption");
//     }

//     pacs->sio = (AA1[10].data[0] == 0x30); // rough check

//     return ERR_NONE;
// }

// ReturnCode picopass_device_parse_wiegand(uint8_t* data, PicopassWiegandRecord* record) {
//     uint32_t* halves = (uint32_t*)data;
//     if(halves[0] == 0) {
//         uint8_t leading0s = __builtin_clz(REVERSE_BYTES_U32(halves[1]));
//         record->bitLength = 31 - leading0s;
//     } else {
//         uint8_t leading0s = __builtin_clz(REVERSE_BYTES_U32(halves[0]));
//         record->bitLength = 63 - leading0s;
//     }
//     FURI_LOG_D(TAG, "bitLength: %d", record->bitLength);

//     if(record->bitLength == 26) {
//         uint8_t* v4 = data + 4;
//         uint32_t bot = v4[3] | (v4[2] << 8) | (v4[1] << 16) | (v4[0] << 24);

//         record->CardNumber = (bot >> 1) & 0xFFFF;
//         record->FacilityCode = (bot >> 17) & 0xFF;
//         FURI_LOG_D(TAG, "FC: %u CN: %u", record->FacilityCode, record->CardNumber);
//         record->valid = true;
//     } else {
//         record->CardNumber = 0;
//         record->FacilityCode = 0;
//         record->valid = false;
//     }
//     return ERR_NONE;
// }
