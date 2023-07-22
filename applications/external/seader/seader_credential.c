#include "seader_credential.h"

#include <toolbox/path.h>
#include <flipper_format/flipper_format.h>
#include "seader_icons.h"

#include <toolbox/protocols/protocol_dict.h>
#include <lfrfid/protocols/lfrfid_protocols.h>
#include <lfrfid/lfrfid_dict_file.h>

#define TAG "SeaderCredential"
#define PICOPASS_BLOCK_LEN 8

#define CSN_INDEX 0
#define CFG_INDEX 1
#define EPURSE_INDEX 2
#define KD_INDEX 3
#define KC_INDEX 4
#define AIA_INDEX 5
#define PACS_CFG_INDEX 6
#define PACS_INDEX 7
#define SR_SIO_INDEX 10

static const char* seader_file_header = "Flipper Seader Credential";
static const uint32_t seader_file_version = 1;

SeaderCredential* seader_credential_alloc() {
    SeaderCredential* seader_dev = malloc(sizeof(SeaderCredential));
    seader_dev->credential = 0;
    seader_dev->bit_length = 0;
    seader_dev->storage = furi_record_open(RECORD_STORAGE);
    seader_dev->dialogs = furi_record_open(RECORD_DIALOGS);
    seader_dev->load_path = furi_string_alloc();
    return seader_dev;
}

void seader_credential_free(SeaderCredential* seader_dev) {
    furi_assert(seader_dev);
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_DIALOGS);
    furi_string_free(seader_dev->load_path);
    free(seader_dev);
}

void seader_credential_set_name(SeaderCredential* cred, const char* name) {
    furi_assert(cred);

    strlcpy(cred->name, name, SEADER_CRED_NAME_MAX_LEN);
}

static bool seader_credential_load(SeaderCredential* cred, FuriString* path, bool show_dialog) {
    bool parsed = false;
    FlipperFormat* file = flipper_format_file_alloc(cred->storage);
    FuriString* temp_str;
    temp_str = furi_string_alloc();
    bool deprecated_version = false;
    cred->type = SeaderCredentialTypeNone;

    if(cred->loading_cb) {
        cred->loading_cb(cred->loading_cb_ctx, true);
    }

    do {
        if(!flipper_format_file_open_existing(file, furi_string_get_cstr(path))) break;

        // Read and verify file header
        uint32_t version = 0;
        if(!flipper_format_read_header(file, temp_str, &version)) break;
        if(furi_string_cmp_str(temp_str, seader_file_header) || (version != seader_file_version)) {
            deprecated_version = true;
            break;
        }

        if(!flipper_format_read_uint32(file, "Bits", (uint32_t*)&(cred->bit_length), 1)) break;
        if(!flipper_format_read_hex(
               file, "Credential", (uint8_t*)&cred->credential, sizeof(cred->credential)))
            break;
        // The order is reversed for storage and for the user opening the file
        uint64_t swapped = __builtin_bswap64(cred->credential);
        cred->credential = swapped;
        // Optional SIO/Diversifier
        flipper_format_read_hex(file, "SIO", cred->sio, sizeof(cred->sio));
        flipper_format_read_hex(file, "Diversifier", cred->diversifier, sizeof(cred->diversifier));
        parsed = true;
    } while(false);

    if(cred->loading_cb) {
        cred->loading_cb(cred->loading_cb_ctx, false);
    }

    if((!parsed) && (show_dialog)) {
        if(deprecated_version) {
            dialog_message_show_storage_error(cred->dialogs, "File format deprecated");
        } else {
            dialog_message_show_storage_error(cred->dialogs, "Can not parse\nfile");
        }
    }
    FURI_LOG_I(TAG, "PACS: (%d) %016llx", cred->bit_length, cred->credential);

    furi_string_free(temp_str);
    flipper_format_free(file);

    return parsed;
}

bool seader_credential_save_mfc(SeaderCredential* cred, const char* name) {
    furi_assert(cred);

    static const char* nfc_file_header = "Flipper NFC device";
    static const uint32_t nfc_file_version = 3;
    static const uint32_t nfc_mifare_classic_data_format_version = 2;

    uint8_t uid[4] = {0xDF, 0xC6, 0x9C, 0x05};
    uint8_t atqa[2] = {0x00, 0x04};
    uint8_t sak = 0x08;
    uint8_t manuf_block[16] = {
        0xDF,
        0xC6,
        0x9C,
        0x05,
        0x80,
        0x08,
        0x04,
        0x00,
        0x00,
        0x00,
        0x73,
        0x65,
        0x61,
        0x64,
        0x65,
        0x72};
    uint8_t sector0_trailer[16] = {
        0xa0,
        0xa1,
        0xa2,
        0xa3,
        0xa4,
        0xa5,
        0x78,
        0x77,
        0x88,
        0xc1,
        0x89,
        0xec,
        0xa9,
        0x7f,
        0x8c,
        0x2a};
    uint8_t sector1_trailer[16] = {
        0x48,
        0x49,
        0x44,
        0x20,
        0x49,
        0x53,
        0x78,
        0x77,
        0x88,
        0xaa,
        0x20,
        0x47,
        0x52,
        0x45,
        0x41,
        0x54};
    uint8_t sectorn_trailer[16] = {
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0x07,
        0x80,
        0x69,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff};
    uint8_t mad_block[16] = {
        0x1b,
        0x01,
        0x4d,
        0x48,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00};
    uint8_t empty_block[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t pacs_block[16] = {0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    bool use_load_path = true;
    bool saved = false;
    FlipperFormat* file = flipper_format_file_alloc(cred->storage);
    FuriString* temp_str;
    temp_str = furi_string_alloc();

    uint64_t sentinel = 1ULL << cred->bit_length;
    uint64_t swapped = __builtin_bswap64(cred->credential | sentinel);
    memcpy(pacs_block + 8, &swapped, sizeof(swapped));

    do {
        if(use_load_path && !furi_string_empty(cred->load_path)) {
            // Get directory name
            path_extract_dirname(furi_string_get_cstr(cred->load_path), temp_str);
            // Make path to file to save
            furi_string_cat_printf(temp_str, "/%s%s", name, SEADER_APP_MFC_EXTENSION);
        } else {
            furi_string_printf(
                temp_str, "%s/%s%s", SEADER_APP_MFC_FOLDER, name, SEADER_APP_MFC_EXTENSION);
        }

        FURI_LOG_D(TAG, "Save as MFC [%s]", furi_string_get_cstr(temp_str));

        // Open file
        if(!flipper_format_file_open_always(file, furi_string_get_cstr(temp_str))) break;
        if(!flipper_format_write_header_cstr(file, nfc_file_header, nfc_file_version)) break;
        // Write nfc device type
        if(!flipper_format_write_comment_cstr(
               file, "Nfc device type can be UID, Mifare Ultralight, Mifare Classic or ISO15693"))
            break;
        furi_string_set(temp_str, "Mifare Classic");
        if(!flipper_format_write_string(file, "Device type", temp_str)) break;
        // Write UID
        if(!flipper_format_write_comment_cstr(file, "UID is common for all formats")) break;
        if(!flipper_format_write_hex(file, "UID", uid, 4)) break;
        // Write ATQA, SAK
        if(!flipper_format_write_comment_cstr(file, "ISO14443 specific fields")) break;
        // Save ATQA in MSB order for correct companion apps display
        if(!flipper_format_write_hex(file, "ATQA", atqa, 2)) break;
        if(!flipper_format_write_hex(file, "SAK", &sak, 1)) break;
        if(!flipper_format_write_comment_cstr(file, "Mifare Classic specific data")) break;
        if(!flipper_format_write_comment_cstr(file, "Made with Seader")) break;
        if(!flipper_format_write_string_cstr(file, "Mifare Classic type", "1K")) break;
        uint8_t blocks = 64;

        if(!flipper_format_write_uint32(
               file, "Data format version", &nfc_mifare_classic_data_format_version, 1))
            break;
        if(!flipper_format_write_comment_cstr(
               file, "Mifare Classic blocks, \'??\' means unknown data"))
            break;
        bool block_saved = true;
        FuriString* block_str;
        block_str = furi_string_alloc();
        for(size_t i = 0; i < blocks; i++) {
            furi_string_printf(temp_str, "Block %d", i);
            switch(i) {
            case 0:
                if(!flipper_format_write_hex(
                       file, furi_string_get_cstr(temp_str), manuf_block, sizeof(manuf_block))) {
                    block_saved = false;
                }
                break;
            case 1:
                if(!flipper_format_write_hex(
                       file, furi_string_get_cstr(temp_str), mad_block, sizeof(mad_block))) {
                    block_saved = false;
                }
                break;
            case 3:
                if(!flipper_format_write_hex(
                       file,
                       furi_string_get_cstr(temp_str),
                       sector0_trailer,
                       sizeof(sector0_trailer))) {
                    block_saved = false;
                }
                break;
            case 5:
                if(!flipper_format_write_hex(
                       file, furi_string_get_cstr(temp_str), pacs_block, sizeof(pacs_block))) {
                    block_saved = false;
                }
                break;
            case 7:
                if(!flipper_format_write_hex(
                       file,
                       furi_string_get_cstr(temp_str),
                       sector1_trailer,
                       sizeof(sector1_trailer))) {
                    block_saved = false;
                }
                break;
            // Trailers
            case 11:
            case 15:
            case 19:
            case 23:
            case 27:
            case 31:
            case 35:
            case 39:
            case 43:
            case 47:
            case 51:
            case 55:
            case 59:
            case 63:
                if(!flipper_format_write_hex(
                       file,
                       furi_string_get_cstr(temp_str),
                       sectorn_trailer,
                       sizeof(sectorn_trailer))) {
                    block_saved = false;
                }
                break;
            default:
                if(!flipper_format_write_hex(
                       file, furi_string_get_cstr(temp_str), empty_block, sizeof(empty_block))) {
                    block_saved = false;
                }
                break;
            }
        }
        furi_string_free(block_str);
        if(!block_saved) break;

        saved = true;
    } while(false);

    if(!saved) {
        dialog_message_show_storage_error(cred->dialogs, "Can not save\nfile");
    }
    furi_string_free(temp_str);
    flipper_format_free(file);
    return saved;
}

bool seader_credential_save_agnostic(SeaderCredential* cred, const char* name) {
    furi_assert(cred);

    bool use_load_path = true;
    bool saved = false;
    FlipperFormat* file = flipper_format_file_alloc(cred->storage);
    FuriString* temp_str;
    temp_str = furi_string_alloc();

    do {
        if(use_load_path && !furi_string_empty(cred->load_path)) {
            // Get directory name
            path_extract_dirname(furi_string_get_cstr(cred->load_path), temp_str);
            // Make path to file to save
            furi_string_cat_printf(temp_str, "/%s%s", name, SEADER_APP_EXTENSION);
        } else {
            furi_string_printf(
                temp_str, "%s/%s%s", STORAGE_APP_DATA_PATH_PREFIX, name, SEADER_APP_EXTENSION);
        }
        // Open file
        if(!flipper_format_file_open_always(file, furi_string_get_cstr(temp_str))) break;

        if(!flipper_format_write_header_cstr(file, seader_file_header, seader_file_version)) break;
        if(!flipper_format_write_uint32(file, "Bits", (uint32_t*)&cred->bit_length, 1)) break;
        uint64_t swapped = __builtin_bswap64(cred->credential);
        if(!flipper_format_write_hex(
               file, "Credential", (uint8_t*)&swapped, sizeof(cred->credential)))
            break;
        if(cred->sio[0] == 0x30) {
            if(!flipper_format_write_hex(file, "SIO", cred->sio, sizeof(cred->sio))) break;
            if(!flipper_format_write_hex(
                   file, "Diversifier", cred->diversifier, sizeof(cred->diversifier)))
                break;
        }

        saved = true;
    } while(false);

    if(!saved) {
        dialog_message_show_storage_error(cred->dialogs, "Can not save\nfile");
    }
    furi_string_free(temp_str);
    flipper_format_free(file);
    return saved;
}

bool seader_credential_save(SeaderCredential* cred, const char* name) {
    uint8_t zero[PICOPASS_BLOCK_LEN] = {0};
    uint8_t csn[PICOPASS_BLOCK_LEN] = {0x7a, 0xf5, 0x31, 0x13, 0xfe, 0xff, 0x12, 0xe0};
    uint8_t cfg[PICOPASS_BLOCK_LEN] = {0x12, 0xff, 0xff, 0xff, 0x7f, 0x1f, 0xff, 0x3c};
    uint8_t epurse[PICOPASS_BLOCK_LEN] = {0xff, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xff};
    uint8_t debit_key[PICOPASS_BLOCK_LEN] = {0xe3, 0xf3, 0x07, 0x84, 0x4a, 0x0b, 0x62, 0x04};
    uint8_t aia[PICOPASS_BLOCK_LEN] = {0xFF, 0xff, 0xff, 0xff, 0xFF, 0xFf, 0xff, 0xFF};
    uint8_t pacs_cfg[PICOPASS_BLOCK_LEN] = {0x03, 0x03, 0x03, 0x03, 0x00, 0x03, 0xe0, 0x14};

    if(cred->save_format == SeaderCredentialSaveFormatAgnostic) {
        return seader_credential_save_agnostic(cred, name);
    } else if(cred->save_format == SeaderCredentialSaveFormatMFC) {
        return seader_credential_save_mfc(cred, name);
    } else if(
        cred->save_format == SeaderCredentialSaveFormatPicopass ||
        cred->save_format == SeaderCredentialSaveFormatSR) {
        bool use_load_path = true;
        bool saved = false;
        bool withSIO = cred->save_format == SeaderCredentialSaveFormatSR;
        FlipperFormat* file = flipper_format_file_alloc(cred->storage);
        FuriString* temp_str = furi_string_alloc();

        if(use_load_path && !furi_string_empty(cred->load_path)) {
            // Get directory name
            path_extract_dirname(furi_string_get_cstr(cred->load_path), temp_str);
            // Make path to file to save
            furi_string_cat_printf(temp_str, "/%s%s", name, ".picopass");
        } else {
            furi_string_printf(
                temp_str, "%s/%s%s", STORAGE_APP_DATA_PATH_PREFIX, name, ".picopass");
        }

        FURI_LOG_D(TAG, "Save as Picopass [%s]", furi_string_get_cstr(temp_str));
        uint64_t sentinel = 1ULL << cred->bit_length;
        uint64_t swapped = __builtin_bswap64(cred->credential | sentinel);
        // FURI_LOG_D(TAG, "PACS: (%d) %016llx | %016llx => %016llx", cred->bit_length, cred->credential, sentinel, swapped);
        do {
            if(!flipper_format_file_open_always(file, furi_string_get_cstr(temp_str))) break;
            if(!flipper_format_write_header_cstr(file, "Flipper Picopass device", 1)) break;
            if(!flipper_format_write_comment_cstr(
                   file, "Picopass blocks generated from Seader app"))
                break;

            bool block_saved = true;
            for(size_t i = 0; i < 20; i++) {
                furi_string_printf(temp_str, "Block %d", i);
                switch(i) {
                case CSN_INDEX:
                    if(withSIO) {
                        if(!flipper_format_write_hex(
                               file,
                               furi_string_get_cstr(temp_str),
                               cred->diversifier,
                               PICOPASS_BLOCK_LEN)) {
                            block_saved = false;
                        }
                    } else {
                        if(!flipper_format_write_hex(
                               file, furi_string_get_cstr(temp_str), csn, sizeof(csn))) {
                            block_saved = false;
                        }
                    }
                    break;
                case EPURSE_INDEX:
                    if(!flipper_format_write_hex(
                           file, furi_string_get_cstr(temp_str), epurse, PICOPASS_BLOCK_LEN)) {
                        block_saved = false;
                    }
                    break;
                case KD_INDEX:
                    if(!flipper_format_write_hex(
                           file, furi_string_get_cstr(temp_str), debit_key, PICOPASS_BLOCK_LEN)) {
                        block_saved = false;
                    }
                    break;
                case AIA_INDEX:
                    if(!flipper_format_write_hex(
                           file, furi_string_get_cstr(temp_str), aia, PICOPASS_BLOCK_LEN)) {
                        block_saved = false;
                    }
                    break;
                case CFG_INDEX:
                    if(!flipper_format_write_hex(
                           file, furi_string_get_cstr(temp_str), cfg, sizeof(cfg))) {
                        block_saved = false;
                    }
                    break;
                case PACS_CFG_INDEX:
                    if(withSIO) {
                        pacs_cfg[0] = 0xA3;
                    }
                    if(!flipper_format_write_hex(
                           file, furi_string_get_cstr(temp_str), pacs_cfg, sizeof(pacs_cfg))) {
                        block_saved = false;
                    }
                    break;
                case PACS_INDEX:
                    if(!flipper_format_write_hex(
                           file,
                           furi_string_get_cstr(temp_str),
                           (uint8_t*)&swapped,
                           PICOPASS_BLOCK_LEN)) {
                        block_saved = false;
                    }
                    break;
                case SR_SIO_INDEX:
                case SR_SIO_INDEX + 1:
                case SR_SIO_INDEX + 2:
                case SR_SIO_INDEX + 3:
                case SR_SIO_INDEX + 4:
                case SR_SIO_INDEX + 5:
                case SR_SIO_INDEX + 6:
                case SR_SIO_INDEX + 7:
                    if(withSIO) {
                        if(!flipper_format_write_hex(
                               file,
                               furi_string_get_cstr(temp_str),
                               cred->sio + ((i - SR_SIO_INDEX) * PICOPASS_BLOCK_LEN),
                               PICOPASS_BLOCK_LEN)) {
                            block_saved = false;
                        }
                    } else {
                        if(!flipper_format_write_hex(
                               file, furi_string_get_cstr(temp_str), zero, sizeof(zero))) {
                            block_saved = false;
                        }
                    }
                    break;
                default:
                    if(!flipper_format_write_hex(
                           file, furi_string_get_cstr(temp_str), zero, sizeof(zero))) {
                        block_saved = false;
                    }
                    break;
                };
                if(!block_saved) {
                    break;
                }
            }
            saved = true;
        } while(false);

        if(!saved) {
            dialog_message_show_storage_error(cred->dialogs, "Can not save\nfile");
        }

        furi_string_free(temp_str);
        flipper_format_free(file);
        return saved;
    } else if(cred->save_format == SeaderCredentialSaveFormatRFID) {
        bool result = false;
        FuriString* file_path = furi_string_alloc();
        furi_string_printf(file_path, "%s/%s%s", ANY_PATH("lfrfid"), name, ".rfid");
        ProtocolDict* dict = protocol_dict_alloc(lfrfid_protocols, LFRFIDProtocolMax);
        ProtocolId protocol = LFRFIDProtocolHidGeneric;

        uint64_t target = 0;
        if(cred->bit_length == 26) {
            //3 bytes
            protocol = LFRFIDProtocolH10301;
            // Remove parity
            target = (cred->credential >> 1) & 0xFFFFFF;
            // Reverse order since it'll get reversed again
            target = __builtin_bswap64(target) >> (64 - 24);
        } else if(cred->bit_length <= 43) {
            //6 bytes
            protocol = LFRFIDProtocolHidGeneric;
            target = cred->credential;
            target = __builtin_bswap64(target) >> (64 - 48);
        } else {
            //8 bytes
            protocol = LFRFIDProtocolHidExGeneric;
            target = cred->credential;
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

        result = lfrfid_dict_file_save(dict, protocol, furi_string_get_cstr(file_path));
        if(result) {
            FURI_LOG_D(TAG, "Written: %d", result);
        } else {
            FURI_LOG_D(TAG, "Failed to write");
        }

        furi_string_free(file_path);
        protocol_dict_free(dict);
        return result;
    }
    return false;
}

bool seader_file_select(SeaderCredential* cred) {
    furi_assert(cred);

    FuriString* seader_app_folder = furi_string_alloc_set(STORAGE_APP_DATA_PATH_PREFIX);

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, SEADER_APP_EXTENSION, &I_Nfc_10px);
    browser_options.base_path = STORAGE_APP_DATA_PATH_PREFIX;

    bool res = dialog_file_browser_show(
        cred->dialogs, cred->load_path, seader_app_folder, &browser_options);

    furi_string_free(seader_app_folder);
    if(res) {
        FuriString* filename;
        filename = furi_string_alloc();
        path_extract_filename(cred->load_path, filename, true);
        strncpy(cred->name, furi_string_get_cstr(filename), SEADER_CRED_NAME_MAX_LEN);
        res = seader_credential_load(cred, cred->load_path, true);
        if(res) {
            seader_credential_set_name(cred, cred->name);
        }
        furi_string_free(filename);
    }

    return res;
}

void seader_credential_clear(SeaderCredential* cred) {
    furi_assert(cred);
    memset(cred->name, 0, sizeof(cred->name));
    cred->credential = 0;
    cred->bit_length = 0;
    cred->type = SeaderCredentialTypeNone;
    memset(cred->sio, 0, sizeof(cred->sio));
    memset(cred->diversifier, 0, sizeof(cred->diversifier));
    furi_string_reset(cred->load_path);
}

bool seader_credential_delete(SeaderCredential* cred, bool use_load_path) {
    furi_assert(cred);

    bool deleted = false;
    FuriString* file_path;
    file_path = furi_string_alloc();

    do {
        // Delete original file
        if(use_load_path && !furi_string_empty(cred->load_path)) {
            furi_string_set(file_path, cred->load_path);
        } else {
            furi_string_printf(file_path, APP_DATA_PATH("%s%s"), cred->name, SEADER_APP_EXTENSION);
        }
        if(!storage_simply_remove(cred->storage, furi_string_get_cstr(file_path))) break;
        deleted = true;
    } while(0);

    if(!deleted) {
        dialog_message_show_storage_error(cred->dialogs, "Can not remove file");
    }

    furi_string_free(file_path);
    return deleted;
}

void seader_credential_set_loading_callback(
    SeaderCredential* cred,
    SeaderLoadingCallback callback,
    void* context) {
    furi_assert(cred);

    cred->loading_cb = callback;
    cred->loading_cb_ctx = context;
}
