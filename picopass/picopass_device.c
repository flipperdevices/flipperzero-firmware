#include "picopass_device.h"

#include <toolbox/path.h>
#include <flipper_format/flipper_format.h>
#include <picopass_icons.h>

#include <toolbox/protocols/protocol_dict.h>
#include <lfrfid/protocols/lfrfid_protocols.h>
#include <lfrfid/lfrfid_dict_file.h>

#include <assets_icons.h>

#define TAG "PicopassDevice"

static const char* picopass_file_header = "Flipper Picopass device";
static const uint32_t picopass_file_version = 1;

const uint8_t picopass_iclass_decryptionkey[] =
    {0xb4, 0x21, 0x2c, 0xca, 0xb7, 0xed, 0x21, 0x0f, 0x7b, 0x93, 0xd4, 0x59, 0x39, 0xc7, 0xdd, 0x36};

PicopassDevice* picopass_device_alloc() {
    PicopassDevice* picopass_dev = malloc(sizeof(PicopassDevice));
    picopass_dev->dev_data.pacs.legacy = false;
    picopass_dev->dev_data.pacs.se_enabled = false;
    picopass_dev->dev_data.pacs.elite_kdf = false;
    picopass_dev->dev_data.pacs.pin_length = 0;
    picopass_dev->storage = furi_record_open(RECORD_STORAGE);
    picopass_dev->dialogs = furi_record_open(RECORD_DIALOGS);
    picopass_dev->load_path = furi_string_alloc();
    return picopass_dev;
}

void picopass_device_set_name(PicopassDevice* dev, const char* name) {
    furi_assert(dev);

    strlcpy(dev->dev_name, name, sizeof(dev->dev_name));
}

// For use with Seader's virtual card processing.
static bool picopass_device_save_file_seader(
    PicopassDevice* dev,
    FlipperFormat* file,
    FuriString* file_path) {
    furi_assert(dev);
    PicopassPacs* pacs = &dev->dev_data.pacs;
    PicopassBlock* AA1 = dev->dev_data.AA1;
    bool result = false;

    const char* seader_file_header = "Flipper Seader Credential";
    const uint32_t seader_file_version = 1;

    do {
        FURI_LOG_D(
            TAG,
            "Save %s %ld to %s",
            seader_file_header,
            seader_file_version,
            furi_string_get_cstr(file_path));
        storage_simply_mkdir(dev->storage, EXT_PATH("apps_data/seader"));
        if(!flipper_format_file_open_always(file, furi_string_get_cstr(file_path))) break;
        if(!flipper_format_write_header_cstr(file, seader_file_header, seader_file_version)) break;
        if(!flipper_format_write_uint32(file, "Bits", (uint32_t*)&pacs->bitLength, 1)) break;
        if(!flipper_format_write_hex(file, "Credential", pacs->credential, PICOPASS_BLOCK_LEN))
            break;

        // Seader only captures 64 byte SIO so I'm going to leave it at that
        uint8_t sio[64];

        // TODO: save SR vs SE more properly
        if(pacs->sio) { // SR
            for(uint8_t i = 0; i < 8; i++) {
                memcpy(sio + (i * 8), AA1[10 + i].data, PICOPASS_BLOCK_LEN);
            }
            if(!flipper_format_write_hex(file, "SIO", sio, sizeof(sio))) break;
        } else if(pacs->se_enabled) { //SE
            for(uint8_t i = 0; i < 8; i++) {
                memcpy(sio + (i * 8), AA1[6 + i].data, PICOPASS_BLOCK_LEN);
            }
            if(!flipper_format_write_hex(file, "SIO", sio, sizeof(sio))) break;
        }
        if(!flipper_format_write_hex(
               file, "Diversifier", AA1[PICOPASS_CSN_BLOCK_INDEX].data, PICOPASS_BLOCK_LEN))
            break;

        result = true;
    } while(false);

    return result;
}

static bool picopass_device_save_file_lfrfid(PicopassDevice* dev, FuriString* file_path) {
    furi_assert(dev);
    PicopassPacs* pacs = &dev->dev_data.pacs;
    ProtocolDict* dict = protocol_dict_alloc(lfrfid_protocols, LFRFIDProtocolMax);
    ProtocolId protocol = LFRFIDProtocolHidGeneric;

    bool result = false;
    uint64_t target = 0;
    uint64_t sentinel = 1ULL << pacs->bitLength;
    memcpy(&target, pacs->credential, PICOPASS_BLOCK_LEN);
    target = __builtin_bswap64(target);
    FURI_LOG_D(TAG, "Original (%d): %016llx", pacs->bitLength, target);

    if(pacs->bitLength == 26) {
        //3 bytes
        protocol = LFRFIDProtocolH10301;
        // Remove parity
        target = (target >> 1) & 0xFFFFFF;
        // Reverse order since it'll get reversed again
        target = __builtin_bswap64(target) >> (64 - 24);
    } else if(pacs->bitLength < 44) {
        // https://gist.github.com/blark/e8f125e402f576bdb7e2d7b3428bdba6
        protocol = LFRFIDProtocolHidGeneric;
        if(pacs->bitLength <= 36) {
            uint64_t header = 1ULL << 37;
            target = __builtin_bswap64((target | sentinel | header) << 4) >> (64 - 48);
        } else {
            target = __builtin_bswap64((target | sentinel) << 4) >> (64 - 48);
        }
    } else {
        //8 bytes
        protocol = LFRFIDProtocolHidExGeneric;
        target = __builtin_bswap64(target);
    }

    size_t data_size = protocol_dict_get_data_size(dict, protocol);
    uint8_t* data = malloc(data_size);
    if(data_size < 8) {
        memcpy(data, (void*)&target, data_size);
    } else {
        // data_size 12 for LFRFIDProtocolHidExGeneric
        memcpy(data + 4, (void*)&target, 8);
    }

    protocol_dict_set_data(dict, protocol, data, data_size);
    free(data);

    FuriString* briefStr;
    briefStr = furi_string_alloc();
    protocol_dict_render_brief_data(dict, briefStr, protocol);
    FURI_LOG_D(TAG, "LFRFID Brief: %s", furi_string_get_cstr(briefStr));
    furi_string_free(briefStr);

    result = lfrfid_dict_file_save(dict, protocol, furi_string_get_cstr(file_path));
    if(result) {
        FURI_LOG_D(TAG, "Written: %d", result);
    } else {
        FURI_LOG_D(TAG, "Failed to write");
    }

    protocol_dict_free(dict);
    return result;
}

static bool picopass_device_save_file(
    PicopassDevice* dev,
    const char* dev_name,
    const char* folder,
    const char* extension,
    bool use_load_path) {
    furi_assert(dev);
    FURI_LOG_D(TAG, "Save File");

    bool saved = false;
    FlipperFormat* file = flipper_format_file_alloc(dev->storage);
    PicopassPacs* pacs = &dev->dev_data.pacs;
    PicopassBlock* AA1 = dev->dev_data.AA1;
    FuriString* temp_str;
    temp_str = furi_string_alloc();

    if(dev->format == PicopassDeviceSaveFormatPartial) {
        // Clear key that may have been set when doing key tests for legacy
        memset(AA1[PICOPASS_SECURE_KD_BLOCK_INDEX].data, 0, PICOPASS_BLOCK_LEN);
    }

    do {
        if(use_load_path && !furi_string_empty(dev->load_path)) {
            // Get directory name
            path_extract_dirname(furi_string_get_cstr(dev->load_path), temp_str);
            // Make path to file to save
            furi_string_cat_printf(temp_str, "/%s%s", dev_name, extension);
        } else {
            // First remove picopass device file if it was saved
            furi_string_printf(temp_str, "%s/%s%s", folder, dev_name, extension);
        }

        if(dev->format == PicopassDeviceSaveFormatHF ||
           dev->format == PicopassDeviceSaveFormatPartial) {
            // Open file
            if(!flipper_format_file_open_always(file, furi_string_get_cstr(temp_str))) break;

            // Write header
            if(!flipper_format_write_header_cstr(file, picopass_file_header, picopass_file_version))
                break;
            if(!flipper_format_write_hex(file, "Credential", pacs->credential, PICOPASS_BLOCK_LEN))
                break;

            // TODO: Add elite
            if(!flipper_format_write_comment_cstr(file, "Picopass blocks")) break;
            bool block_saved = true;

            size_t app_limit = AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[0] < PICOPASS_MAX_APP_LIMIT ?
                                   AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[0] :
                                   PICOPASS_MAX_APP_LIMIT;
            for(size_t i = 0; i < app_limit; i++) {
                furi_string_printf(temp_str, "Block %d", i);
                if(!flipper_format_write_hex(
                       file, furi_string_get_cstr(temp_str), AA1[i].data, PICOPASS_BLOCK_LEN)) {
                    block_saved = false;
                    break;
                }
            }
            if(!block_saved) break;
            saved = true;
        } else if(dev->format == PicopassDeviceSaveFormatLF) {
            saved = picopass_device_save_file_lfrfid(dev, temp_str);
        } else if(dev->format == PicopassDeviceSaveFormatSeader) {
            saved = picopass_device_save_file_seader(dev, file, temp_str);
        }
    } while(false);

    if(!saved) {
        dialog_message_show_storage_error(dev->dialogs, "Can not save\nfile");
    }
    furi_string_free(temp_str);
    flipper_format_free(file);
    return saved;
}

bool picopass_device_save(PicopassDevice* dev, const char* dev_name) {
    if(dev->format == PicopassDeviceSaveFormatHF) {
        return picopass_device_save_file(
            dev, dev_name, STORAGE_APP_DATA_PATH_PREFIX, PICOPASS_APP_EXTENSION, true);
    } else if(dev->format == PicopassDeviceSaveFormatLF) {
        return picopass_device_save_file(dev, dev_name, ANY_PATH("lfrfid"), ".rfid", true);
    } else if(dev->format == PicopassDeviceSaveFormatSeader) {
        return picopass_device_save_file(
            dev, dev_name, EXT_PATH("apps_data/seader"), ".credential", true);
    } else if(dev->format == PicopassDeviceSaveFormatPartial) {
        return picopass_device_save_file(
            dev, dev_name, STORAGE_APP_DATA_PATH_PREFIX, PICOPASS_APP_EXTENSION, true);
    }

    return false;
}

static bool picopass_device_load_data(PicopassDevice* dev, FuriString* path, bool show_dialog) {
    bool parsed = false;
    FlipperFormat* file = flipper_format_file_alloc(dev->storage);
    PicopassBlock* AA1 = dev->dev_data.AA1;
    PicopassPacs* pacs = &dev->dev_data.pacs;
    FuriString* temp_str;
    temp_str = furi_string_alloc();
    bool deprecated_version = false;

    if(dev->loading_cb) {
        dev->loading_cb(dev->loading_cb_ctx, true);
    }

    do {
        if(!flipper_format_file_open_existing(file, furi_string_get_cstr(path))) break;

        // Read and verify file header
        uint32_t version = 0;
        if(!flipper_format_read_header(file, temp_str, &version)) break;
        if(furi_string_cmp_str(temp_str, picopass_file_header) ||
           (version != picopass_file_version)) {
            deprecated_version = true;
            break;
        }

        // Parse header blocks
        bool block_read = true;
        for(size_t i = 0; i < 6; i++) {
            furi_string_printf(temp_str, "Block %d", i);
            if(!flipper_format_read_hex(
                   file, furi_string_get_cstr(temp_str), AA1[i].data, PICOPASS_BLOCK_LEN)) {
                block_read = false;
                break;
            }
        }

        size_t app_limit = AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[0];
        // Fix for unpersonalized cards that have app_limit set to 0xFF
        if(app_limit > PICOPASS_MAX_APP_LIMIT) app_limit = PICOPASS_MAX_APP_LIMIT;
        for(size_t i = 6; i < app_limit; i++) {
            furi_string_printf(temp_str, "Block %d", i);
            if(!flipper_format_read_hex(
                   file, furi_string_get_cstr(temp_str), AA1[i].data, PICOPASS_BLOCK_LEN)) {
                block_read = false;
                break;
            }
        }
        if(!block_read) break;

        picopass_device_parse_credential(AA1, pacs);
        picopass_device_parse_wiegand(pacs->credential, pacs);

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

void picopass_device_clear(PicopassDevice* dev) {
    furi_assert(dev);

    picopass_device_data_clear(&dev->dev_data);
    memset(&dev->dev_data, 0, sizeof(dev->dev_data));
    dev->format = PicopassDeviceSaveFormatHF;
    furi_string_reset(dev->load_path);
}

void picopass_device_free(PicopassDevice* picopass_dev) {
    furi_assert(picopass_dev);
    picopass_device_clear(picopass_dev);
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_DIALOGS);
    furi_string_free(picopass_dev->load_path);
    free(picopass_dev);
}

bool picopass_file_select(PicopassDevice* dev) {
    furi_assert(dev);

    FuriString* picopass_app_folder;
    picopass_app_folder = furi_string_alloc_set(STORAGE_APP_DATA_PATH_PREFIX);

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, PICOPASS_APP_EXTENSION, &I_Nfc_10px);
    browser_options.base_path = STORAGE_APP_DATA_PATH_PREFIX;

    bool res = dialog_file_browser_show(
        dev->dialogs, dev->load_path, picopass_app_folder, &browser_options);

    furi_string_free(picopass_app_folder);
    if(res) {
        FuriString* filename;
        filename = furi_string_alloc();
        path_extract_filename(dev->load_path, filename, true);
        strlcpy(dev->dev_name, furi_string_get_cstr(filename), sizeof(dev->dev_name));
        res = picopass_device_load_data(dev, dev->load_path, true);
        if(res) {
            picopass_device_set_name(dev, dev->dev_name);
        }
        furi_string_free(filename);
    }

    return res;
}

void picopass_device_data_clear(PicopassDeviceData* dev_data) {
    for(size_t i = 0; i < PICOPASS_MAX_APP_LIMIT; i++) {
        memset(dev_data->AA1[i].data, 0, sizeof(dev_data->AA1[i].data));
    }
    dev_data->pacs.legacy = false;
    dev_data->pacs.se_enabled = false;
    dev_data->pacs.elite_kdf = false;
    dev_data->pacs.pin_length = 0;
}

bool picopass_device_delete(PicopassDevice* dev, bool use_load_path) {
    furi_assert(dev);

    bool deleted = false;
    FuriString* file_path;
    file_path = furi_string_alloc();

    do {
        // Delete original file
        if(use_load_path && !furi_string_empty(dev->load_path)) {
            furi_string_set(file_path, dev->load_path);
        } else {
            furi_string_printf(
                file_path, APP_DATA_PATH("%s%s"), dev->dev_name, PICOPASS_APP_EXTENSION);
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

void picopass_device_set_loading_callback(
    PicopassDevice* dev,
    PicopassLoadingCallback callback,
    void* context) {
    furi_assert(dev);

    dev->loading_cb = callback;
    dev->loading_cb_ctx = context;
}

void picopass_device_decrypt(uint8_t* enc_data, uint8_t* dec_data) {
    uint8_t key[32] = {0};
    memcpy(key, picopass_iclass_decryptionkey, sizeof(picopass_iclass_decryptionkey));
    mbedtls_des3_context ctx;
    mbedtls_des3_init(&ctx);
    mbedtls_des3_set2key_dec(&ctx, key);
    mbedtls_des3_crypt_ecb(&ctx, enc_data, dec_data);
    mbedtls_des3_free(&ctx);
}

void picopass_device_parse_credential(PicopassBlock* AA1, PicopassPacs* pacs) {
    pacs->biometrics = AA1[6].data[4];
    pacs->pin_length = AA1[6].data[6] & 0x0F;
    pacs->encryption = AA1[6].data[7];

    if(pacs->encryption == PicopassDeviceEncryption3DES) {
        FURI_LOG_D(TAG, "3DES Encrypted");
        picopass_device_decrypt(AA1[7].data, pacs->credential);

        picopass_device_decrypt(AA1[8].data, pacs->pin0);

        picopass_device_decrypt(AA1[9].data, pacs->pin1);
    } else if(pacs->encryption == PicopassDeviceEncryptionNone) {
        FURI_LOG_D(TAG, "No Encryption");
        memcpy(pacs->credential, AA1[7].data, PICOPASS_BLOCK_LEN);
        memcpy(pacs->pin0, AA1[8].data, PICOPASS_BLOCK_LEN);
        memcpy(pacs->pin1, AA1[9].data, PICOPASS_BLOCK_LEN);
    } else if(pacs->encryption == PicopassDeviceEncryptionDES) {
        FURI_LOG_D(TAG, "DES Encrypted");
    } else {
        FURI_LOG_D(TAG, "Unknown encryption");
    }

    pacs->sio = (AA1[10].data[0] == 0x30); // rough check
}

void picopass_device_parse_wiegand(uint8_t* credential, PicopassPacs* pacs) {
    uint32_t* halves = (uint32_t*)credential;
    if(halves[0] == 0) {
        uint8_t leading0s = __builtin_clz(REVERSE_BYTES_U32(halves[1]));
        pacs->bitLength = 31 - leading0s;
    } else {
        uint8_t leading0s = __builtin_clz(REVERSE_BYTES_U32(halves[0]));
        pacs->bitLength = 63 - leading0s;
    }

    // Remove sentinel bit from credential.  Byteswapping to handle array of bytes vs 64bit value
    uint64_t sentinel = __builtin_bswap64(1ULL << pacs->bitLength);
    uint64_t swapped = 0;
    memcpy(&swapped, credential, sizeof(uint64_t));
    swapped = swapped ^ sentinel;
    memcpy(credential, &swapped, sizeof(uint64_t));
    FURI_LOG_D(TAG, "PACS: (%d) %016llx", pacs->bitLength, swapped);
}

bool picopass_device_hid_csn(PicopassDevice* dev) {
    furi_assert(dev);
    PicopassBlock* AA1 = dev->dev_data.AA1;
    uint8_t* csn = AA1[PICOPASS_CSN_BLOCK_INDEX].data;
    // From Proxmark3 RRG sourcecode
    bool isHidRange = (memcmp(csn + 5, "\xFF\x12\xE0", 3) == 0) && ((csn[4] & 0xF0) == 0xF0);

    return isHidRange;
}
