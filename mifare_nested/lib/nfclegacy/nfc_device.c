#include "nfc_device.h"
#include "nfc_types.h"

#include <lib/toolbox/path.h>
#include <lib/toolbox/hex.h>
#include "protocols/nfc_util.h"
#include <flipper_format/flipper_format.h>

#define TAG "NfcDevice"
#define NFC_DEVICE_KEYS_FOLDER EXT_PATH("nfc/.cache")
#define NFC_DEVICE_KEYS_EXTENSION ".keys"

static const char* nfc_file_header = "Flipper NFC device";
static const uint32_t nfc_file_version = 3;

static const char* nfc_keys_file_header = "Flipper NFC keys";
static const uint32_t nfc_keys_file_version = 1;

// Protocols format versions
static const uint32_t nfc_mifare_classic_data_format_version = 2;
static const uint32_t nfc_mifare_ultralight_data_format_version = 1;

NfcDevice* nfc_device_alloc() {
    NfcDevice* nfc_dev = malloc(sizeof(NfcDevice));
    nfc_dev->storage = furi_record_open(RECORD_STORAGE);
    nfc_dev->dialogs = furi_record_open(RECORD_DIALOGS);
    nfc_dev->load_path = furi_string_alloc();
    nfc_dev->dev_data.parsed_data = furi_string_alloc();
    nfc_dev->folder = furi_string_alloc();

    return nfc_dev;
}

void nfc_device_free(NfcDevice* nfc_dev) {
    furi_assert(nfc_dev);
    nfc_device_clear(nfc_dev);
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_DIALOGS);
    furi_string_free(nfc_dev->load_path);
    if(nfc_dev->dev_data.parsed_data != NULL) {
        furi_string_free(nfc_dev->dev_data.parsed_data);
    }
    furi_string_free(nfc_dev->folder);
    free(nfc_dev);
}

static void nfc_device_prepare_format_string(NfcDevice* dev, FuriString* format_string) {
    if(dev->format == NfcDeviceSaveFormatUid) {
        furi_string_set(format_string, "UID");
    } else if(dev->format == NfcDeviceSaveFormatBankCard) {
        furi_string_set(format_string, "Bank card");
    } else if(dev->format == NfcDeviceSaveFormatMifareUl) {
        furi_string_set(format_string, nfc_mf_ul_type(dev->dev_data.mf_ul_data.type, true));
    } else if(dev->format == NfcDeviceSaveFormatMifareClassic) {
        furi_string_set(format_string, "Mifare Classic");
    } else if(dev->format == NfcDeviceSaveFormatMifareDesfire) {
        furi_string_set(format_string, "Mifare DESFire");
    } else if(dev->format == NfcDeviceSaveFormatNfcV) {
        furi_string_set(format_string, "ISO15693");
    } else {
        furi_string_set(format_string, "Unknown");
    }
}

static bool nfc_device_parse_format_string(NfcDevice* dev, FuriString* format_string) {
    if(furi_string_start_with_str(format_string, "UID")) {
        dev->format = NfcDeviceSaveFormatUid;
        dev->dev_data.protocol = NfcDeviceProtocolUnknown;
        return true;
    }
    if(furi_string_start_with_str(format_string, "Bank card")) {
        dev->format = NfcDeviceSaveFormatBankCard;
        dev->dev_data.protocol = NfcDeviceProtocolEMV;
        return true;
    }
    // Check Mifare Ultralight types
    for(MfUltralightType type = MfUltralightTypeUnknown; type < MfUltralightTypeNum; type++) {
        if(furi_string_equal(format_string, nfc_mf_ul_type(type, true))) {
            dev->format = NfcDeviceSaveFormatMifareUl;
            dev->dev_data.protocol = NfcDeviceProtocolMifareUl;
            dev->dev_data.mf_ul_data.type = type;
            return true;
        }
    }
    if(furi_string_start_with_str(format_string, "Mifare Classic")) {
        dev->format = NfcDeviceSaveFormatMifareClassic;
        dev->dev_data.protocol = NfcDeviceProtocolMifareClassic;
        return true;
    }
    if(furi_string_start_with_str(format_string, "Mifare DESFire")) {
        dev->format = NfcDeviceSaveFormatMifareDesfire;
        dev->dev_data.protocol = NfcDeviceProtocolMifareDesfire;
        return true;
    }
    if(furi_string_start_with_str(format_string, "ISO15693")) {
        dev->format = NfcDeviceSaveFormatNfcV;
        dev->dev_data.protocol = NfcDeviceProtocolNfcV;
        return true;
    }
    return false;
}

static bool nfc_device_save_mifare_ul_data(FlipperFormat* file, NfcDevice* dev) {
    bool saved = false;
    MfUltralightData* data = &dev->dev_data.mf_ul_data;
    FuriString* temp_str;
    temp_str = furi_string_alloc();

    // Save Mifare Ultralight specific data
    do {
        if(!flipper_format_write_comment_cstr(file, "Mifare Ultralight specific data")) break;
        if(!flipper_format_write_uint32(
               file, "Data format version", &nfc_mifare_ultralight_data_format_version, 1))
            break;
        if(!flipper_format_write_hex(file, "Signature", data->signature, sizeof(data->signature)))
            break;
        if(!flipper_format_write_hex(
               file, "Mifare version", (uint8_t*)&data->version, sizeof(data->version)))
            break;
        // Write conters and tearing flags data
        bool counters_saved = true;
        for(uint8_t i = 0; i < 3; i++) {
            furi_string_printf(temp_str, "Counter %d", i);
            if(!flipper_format_write_uint32(
                   file, furi_string_get_cstr(temp_str), &data->counter[i], 1)) {
                counters_saved = false;
                break;
            }
            furi_string_printf(temp_str, "Tearing %d", i);
            if(!flipper_format_write_hex(
                   file, furi_string_get_cstr(temp_str), &data->tearing[i], 1)) {
                counters_saved = false;
                break;
            }
        }
        if(!counters_saved) break;
        // Write pages data
        uint32_t pages_total = data->data_size / 4;
        if(!flipper_format_write_uint32(file, "Pages total", &pages_total, 1)) break;
        uint32_t pages_read = data->data_read / 4;
        if(!flipper_format_write_uint32(file, "Pages read", &pages_read, 1)) break;
        bool pages_saved = true;
        for(uint16_t i = 0; i < data->data_size; i += 4) {
            furi_string_printf(temp_str, "Page %d", i / 4);
            if(!flipper_format_write_hex(file, furi_string_get_cstr(temp_str), &data->data[i], 4)) {
                pages_saved = false;
                break;
            }
        }
        if(!pages_saved) break;

        // Write authentication counter
        uint32_t auth_counter = data->curr_authlim;
        if(!flipper_format_write_uint32(file, "Failed authentication attempts", &auth_counter, 1))
            break;

        saved = true;
    } while(false);

    furi_string_free(temp_str);
    return saved;
}

bool nfc_device_load_mifare_ul_data(FlipperFormat* file, NfcDevice* dev) {
    bool parsed = false;
    MfUltralightData* data = &dev->dev_data.mf_ul_data;
    FuriString* temp_str;
    temp_str = furi_string_alloc();
    uint32_t data_format_version = 0;

    do {
        // Read Mifare Ultralight format version
        if(!flipper_format_read_uint32(file, "Data format version", &data_format_version, 1)) {
            if(!flipper_format_rewind(file)) break;
        }

        // Read signature
        if(!flipper_format_read_hex(file, "Signature", data->signature, sizeof(data->signature)))
            break;
        // Read Mifare version
        if(!flipper_format_read_hex(
               file, "Mifare version", (uint8_t*)&data->version, sizeof(data->version)))
            break;
        // Read counters and tearing flags
        bool counters_parsed = true;
        for(uint8_t i = 0; i < 3; i++) {
            furi_string_printf(temp_str, "Counter %d", i);
            if(!flipper_format_read_uint32(
                   file, furi_string_get_cstr(temp_str), &data->counter[i], 1)) {
                counters_parsed = false;
                break;
            }
            furi_string_printf(temp_str, "Tearing %d", i);
            if(!flipper_format_read_hex(
                   file, furi_string_get_cstr(temp_str), &data->tearing[i], 1)) {
                counters_parsed = false;
                break;
            }
        }
        if(!counters_parsed) break;
        // Read pages
        uint32_t pages_total = 0;
        if(!flipper_format_read_uint32(file, "Pages total", &pages_total, 1)) break;
        uint32_t pages_read = 0;
        if(data_format_version < nfc_mifare_ultralight_data_format_version) {
            pages_read = pages_total;
        } else {
            if(!flipper_format_read_uint32(file, "Pages read", &pages_read, 1)) break;
        }
        data->data_size = pages_total * 4;
        data->data_read = pages_read * 4;
        if(data->data_size > MF_UL_MAX_DUMP_SIZE || data->data_read > MF_UL_MAX_DUMP_SIZE) break;
        bool pages_parsed = true;
        for(uint16_t i = 0; i < pages_total; i++) {
            furi_string_printf(temp_str, "Page %d", i);
            if(!flipper_format_read_hex(
                   file, furi_string_get_cstr(temp_str), &data->data[i * 4], 4)) {
                pages_parsed = false;
                break;
            }
        }
        if(!pages_parsed) break;

        // Read authentication counter
        uint32_t auth_counter;
        if(!flipper_format_read_uint32(file, "Failed authentication attempts", &auth_counter, 1))
            auth_counter = 0;
        data->curr_authlim = auth_counter;

        data->auth_success = mf_ul_is_full_capture(data);

        parsed = true;
    } while(false);

    furi_string_free(temp_str);
    return parsed;
}

static void nfc_device_write_mifare_classic_block(
    FuriString* block_str,
    MfClassicData* data,
    uint8_t block_num) {
    furi_string_reset(block_str);
    bool is_sec_trailer = mf_classic_is_sector_trailer(block_num);
    if(is_sec_trailer) {
        uint8_t sector_num = mf_classic_get_sector_by_block(block_num);
        MfClassicSectorTrailer* sec_tr = mf_classic_get_sector_trailer_by_sector(data, sector_num);
        // Write key A
        for(size_t i = 0; i < sizeof(sec_tr->key_a); i++) {
            if(mf_classic_is_key_found(data, sector_num, MfClassicKeyA)) {
                furi_string_cat_printf(block_str, "%02X ", sec_tr->key_a[i]);
            } else {
                furi_string_cat_printf(block_str, "?? ");
            }
        }
        // Write Access bytes
        for(size_t i = 0; i < MF_CLASSIC_ACCESS_BYTES_SIZE; i++) {
            if(mf_classic_is_block_read(data, block_num)) {
                furi_string_cat_printf(block_str, "%02X ", sec_tr->access_bits[i]);
            } else {
                furi_string_cat_printf(block_str, "?? ");
            }
        }
        // Write key B
        for(size_t i = 0; i < sizeof(sec_tr->key_b); i++) {
            if(mf_classic_is_key_found(data, sector_num, MfClassicKeyB)) {
                furi_string_cat_printf(block_str, "%02X ", sec_tr->key_b[i]);
            } else {
                furi_string_cat_printf(block_str, "?? ");
            }
        }
    } else {
        // Write data block
        for(size_t i = 0; i < MF_CLASSIC_BLOCK_SIZE; i++) {
            if(mf_classic_is_block_read(data, block_num)) {
                furi_string_cat_printf(block_str, "%02X ", data->block[block_num].value[i]);
            } else {
                furi_string_cat_printf(block_str, "?? ");
            }
        }
    }
    furi_string_trim(block_str);
}

static bool nfc_device_save_mifare_classic_data(FlipperFormat* file, NfcDevice* dev) {
    bool saved = false;
    MfClassicData* data = &dev->dev_data.mf_classic_data;
    FuriString* temp_str;
    temp_str = furi_string_alloc();
    uint16_t blocks = 0;

    // Save Mifare Classic specific data
    do {
        if(!flipper_format_write_comment_cstr(file, "Mifare Classic specific data")) break;

        if(data->type == MfClassicTypeMini) {
            if(!flipper_format_write_string_cstr(file, "Mifare Classic type", "MINI")) break;
            blocks = 20;
        } else if(data->type == MfClassicType1k) {
            if(!flipper_format_write_string_cstr(file, "Mifare Classic type", "1K")) break;
            blocks = 64;
        } else if(data->type == MfClassicType4k) {
            if(!flipper_format_write_string_cstr(file, "Mifare Classic type", "4K")) break;
            blocks = 256;
        }
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
            nfc_device_write_mifare_classic_block(block_str, data, i);
            if(!flipper_format_write_string(file, furi_string_get_cstr(temp_str), block_str)) {
                block_saved = false;
                break;
            }
        }
        furi_string_free(block_str);
        if(!block_saved) break;
        saved = true;
    } while(false);

    furi_string_free(temp_str);
    return saved;
}

static void nfc_device_load_mifare_classic_block(
    FuriString* block_str,
    MfClassicData* data,
    uint8_t block_num) {
    furi_string_trim(block_str);
    MfClassicBlock block_tmp = {};
    bool is_sector_trailer = mf_classic_is_sector_trailer(block_num);
    uint8_t sector_num = mf_classic_get_sector_by_block(block_num);
    uint16_t block_unknown_bytes_mask = 0;

    furi_string_trim(block_str);
    for(size_t i = 0; i < MF_CLASSIC_BLOCK_SIZE; i++) {
        char hi = furi_string_get_char(block_str, 3 * i);
        char low = furi_string_get_char(block_str, 3 * i + 1);
        uint8_t byte = 0;
        if(hex_char_to_uint8(hi, low, &byte)) {
            block_tmp.value[i] = byte;
        } else {
            FURI_BIT_SET(block_unknown_bytes_mask, i);
        }
    }

    if(block_unknown_bytes_mask == 0xffff) {
        // All data is unknown, exit
        return;
    }

    if(is_sector_trailer) {
        MfClassicSectorTrailer* sec_tr_tmp = (MfClassicSectorTrailer*)&block_tmp;
        // Load Key A
        // Key A mask 0b0000000000111111 = 0x003f
        if((block_unknown_bytes_mask & 0x003f) == 0) {
            uint64_t key = nfc_util_bytes2num(sec_tr_tmp->key_a, sizeof(sec_tr_tmp->key_a));
            mf_classic_set_key_found(data, sector_num, MfClassicKeyA, key);
        }
        // Load Access Bits
        // Access bits mask 0b0000001111000000 = 0x03c0
        if((block_unknown_bytes_mask & 0x03c0) == 0) {
            mf_classic_set_block_read(data, block_num, &block_tmp);
        }
        // Load Key B
        // Key B mask 0b1111110000000000 = 0xfc00
        if((block_unknown_bytes_mask & 0xfc00) == 0) {
            uint64_t key = nfc_util_bytes2num(sec_tr_tmp->key_b, sizeof(sec_tr_tmp->key_b));
            mf_classic_set_key_found(data, sector_num, MfClassicKeyB, key);
        }
    } else {
        if(block_unknown_bytes_mask == 0) {
            mf_classic_set_block_read(data, block_num, &block_tmp);
        }
    }
}

static bool nfc_device_load_mifare_classic_data(FlipperFormat* file, NfcDevice* dev) {
    bool parsed = false;
    MfClassicData* data = &dev->dev_data.mf_classic_data;
    FuriString* temp_str;
    uint32_t data_format_version = 0;
    temp_str = furi_string_alloc();
    uint16_t data_blocks = 0;
    memset(data, 0, sizeof(MfClassicData));

    do {
        // Read Mifare Classic type
        if(!flipper_format_read_string(file, "Mifare Classic type", temp_str)) break;
        if(!furi_string_cmp(temp_str, "MINI")) {
            data->type = MfClassicTypeMini;
            data_blocks = 20;
        } else if(!furi_string_cmp(temp_str, "1K")) {
            data->type = MfClassicType1k;
            data_blocks = 64;
        } else if(!furi_string_cmp(temp_str, "4K")) {
            data->type = MfClassicType4k;
            data_blocks = 256;
        } else {
            break;
        }

        bool old_format = false;
        // Read Mifare Classic format version
        if(!flipper_format_read_uint32(file, "Data format version", &data_format_version, 1)) {
            // Load unread sectors with zero keys access for backward compatibility
            if(!flipper_format_rewind(file)) break;
            old_format = true;
        } else {
            if(data_format_version < nfc_mifare_classic_data_format_version) {
                old_format = true;
            }
        }

        // Read Mifare Classic blocks
        bool block_read = true;
        FuriString* block_str;
        block_str = furi_string_alloc();
        for(size_t i = 0; i < data_blocks; i++) {
            furi_string_printf(temp_str, "Block %d", i);
            if(!flipper_format_read_string(file, furi_string_get_cstr(temp_str), block_str)) {
                block_read = false;
                break;
            }
            nfc_device_load_mifare_classic_block(block_str, data, i);
        }
        furi_string_free(block_str);
        if(!block_read) break;

        // Set keys and blocks as unknown for backward compatibility
        if(old_format) {
            data->key_a_mask = 0ULL;
            data->key_b_mask = 0ULL;
            memset(data->block_read_mask, 0, sizeof(data->block_read_mask));
        }

        parsed = true;
    } while(false);

    furi_string_free(temp_str);
    return parsed;
}

static void nfc_device_get_key_cache_file_path(NfcDevice* dev, FuriString* file_path) {
    uint8_t* uid = dev->dev_data.nfc_data.uid;
    uint8_t uid_len = dev->dev_data.nfc_data.uid_len;
    furi_string_set(file_path, NFC_DEVICE_KEYS_FOLDER "/");
    for(size_t i = 0; i < uid_len; i++) {
        furi_string_cat_printf(file_path, "%02X", uid[i]);
    }
    furi_string_cat_printf(file_path, NFC_DEVICE_KEYS_EXTENSION);
}

static bool nfc_device_save_mifare_classic_keys(NfcDevice* dev) {
    FlipperFormat* file = flipper_format_file_alloc(dev->storage);
    MfClassicData* data = &dev->dev_data.mf_classic_data;
    FuriString* temp_str;
    temp_str = furi_string_alloc();

    nfc_device_get_key_cache_file_path(dev, temp_str);
    bool save_success = false;
    do {
        if(!storage_simply_mkdir(dev->storage, NFC_DEVICE_KEYS_FOLDER)) break;
        if(!storage_simply_remove(dev->storage, furi_string_get_cstr(temp_str))) break;
        if(!flipper_format_file_open_always(file, furi_string_get_cstr(temp_str))) break;
        if(!flipper_format_write_header_cstr(file, nfc_keys_file_header, nfc_keys_file_version))
            break;
        if(data->type == MfClassicTypeMini) {
            if(!flipper_format_write_string_cstr(file, "Mifare Classic type", "MINI")) break;
        } else if(data->type == MfClassicType1k) {
            if(!flipper_format_write_string_cstr(file, "Mifare Classic type", "1K")) break;
        } else if(data->type == MfClassicType4k) {
            if(!flipper_format_write_string_cstr(file, "Mifare Classic type", "4K")) break;
        }
        if(!flipper_format_write_hex_uint64(file, "Key A map", &data->key_a_mask, 1)) break;
        if(!flipper_format_write_hex_uint64(file, "Key B map", &data->key_b_mask, 1)) break;
        uint8_t sector_num = mf_classic_get_total_sectors_num(data->type);
        bool key_save_success = true;
        for(size_t i = 0; (i < sector_num) && (key_save_success); i++) {
            MfClassicSectorTrailer* sec_tr = mf_classic_get_sector_trailer_by_sector(data, i);
            if(FURI_BIT(data->key_a_mask, i)) {
                furi_string_printf(temp_str, "Key A sector %d", i);
                key_save_success = flipper_format_write_hex(
                    file, furi_string_get_cstr(temp_str), sec_tr->key_a, 6);
            }
            if(!key_save_success) break;
            if(FURI_BIT(data->key_b_mask, i)) {
                furi_string_printf(temp_str, "Key B sector %d", i);
                key_save_success = flipper_format_write_hex(
                    file, furi_string_get_cstr(temp_str), sec_tr->key_b, 6);
            }
        }
        save_success = key_save_success;
    } while(false);

    flipper_format_free(file);
    furi_string_free(temp_str);
    return save_success;
}

bool nfc_device_load_key_cache(NfcDevice* dev) {
    furi_assert(dev);
    FuriString* temp_str;
    temp_str = furi_string_alloc();

    MfClassicData* data = &dev->dev_data.mf_classic_data;
    nfc_device_get_key_cache_file_path(dev, temp_str);
    FlipperFormat* file = flipper_format_file_alloc(dev->storage);

    bool load_success = false;
    do {
        if(storage_common_stat(dev->storage, furi_string_get_cstr(temp_str), NULL) != FSE_OK)
            break;
        if(!flipper_format_file_open_existing(file, furi_string_get_cstr(temp_str))) break;
        uint32_t version = 0;
        if(!flipper_format_read_header(file, temp_str, &version)) break;
        if(furi_string_cmp_str(temp_str, nfc_keys_file_header)) break;
        if(version != nfc_keys_file_version) break;
        if(!flipper_format_read_string(file, "Mifare Classic type", temp_str)) break;
        if(!furi_string_cmp(temp_str, "MINI")) {
            data->type = MfClassicTypeMini;
        } else if(!furi_string_cmp(temp_str, "1K")) {
            data->type = MfClassicType1k;
        } else if(!furi_string_cmp(temp_str, "4K")) {
            data->type = MfClassicType4k;
        } else {
            break;
        }
        if(!flipper_format_read_hex_uint64(file, "Key A map", &data->key_a_mask, 1)) break;
        if(!flipper_format_read_hex_uint64(file, "Key B map", &data->key_b_mask, 1)) break;
        uint8_t sectors = mf_classic_get_total_sectors_num(data->type);
        bool key_read_success = true;
        for(size_t i = 0; (i < sectors) && (key_read_success); i++) {
            MfClassicSectorTrailer* sec_tr = mf_classic_get_sector_trailer_by_sector(data, i);
            if(FURI_BIT(data->key_a_mask, i)) {
                furi_string_printf(temp_str, "Key A sector %d", i);
                key_read_success = flipper_format_read_hex(
                    file, furi_string_get_cstr(temp_str), sec_tr->key_a, 6);
            }
            if(!key_read_success) break;
            if(FURI_BIT(data->key_b_mask, i)) {
                furi_string_printf(temp_str, "Key B sector %d", i);
                key_read_success = flipper_format_read_hex(
                    file, furi_string_get_cstr(temp_str), sec_tr->key_b, 6);
            }
        }
        load_success = key_read_success;
    } while(false);

    furi_string_free(temp_str);
    flipper_format_free(file);

    return load_success;
}

void nfc_device_set_name(NfcDevice* dev, const char* name) {
    furi_assert(dev);

    strlcpy(dev->dev_name, name, NFC_DEV_NAME_MAX_LEN);
}

static void nfc_device_get_path_without_ext(FuriString* orig_path, FuriString* shadow_path) {
    // TODO: this won't work if there is ".nfc" anywhere in the path other than
    // at the end
    size_t ext_start = furi_string_search(orig_path, NFC_APP_FILENAME_EXTENSION);
    furi_string_set_n(shadow_path, orig_path, 0, ext_start);
}

static void nfc_device_get_shadow_path(FuriString* orig_path, FuriString* shadow_path) {
    nfc_device_get_path_without_ext(orig_path, shadow_path);
    furi_string_cat_printf(shadow_path, "%s", NFC_APP_SHADOW_EXTENSION);
}

static void nfc_device_get_folder_from_path(FuriString* path, FuriString* folder) {
    size_t last_slash = furi_string_search_rchar(path, '/');
    if(last_slash == FURI_STRING_FAILURE) {
        // No slashes in the path, treat the whole path as a folder
        furi_string_set(folder, path);
    } else {
        furi_string_set_n(folder, path, 0, last_slash);
    }
}

bool nfc_device_save(NfcDevice* dev, const char* dev_name) {
    return false;
    furi_assert(dev);

    bool saved = false;
    FlipperFormat* file = flipper_format_file_alloc(dev->storage);
    FurryHalNfcDevData* data = &dev->dev_data.nfc_data;
    FuriString* temp_str;
    temp_str = furi_string_alloc();

    do {
        // Create directory if necessary
        FuriString* folder = furi_string_alloc();
        // Get folder from filename (filename is in the form of "folder/filename.nfc", so the folder is "folder/")
        furi_string_set(temp_str, dev_name);
        // Get folder from filename
        nfc_device_get_folder_from_path(temp_str, folder);
        FURI_LOG_I("Nfc", "Saving to folder %s", furi_string_get_cstr(folder));
        if(!storage_simply_mkdir(dev->storage, furi_string_get_cstr(folder))) {
            FURI_LOG_E("Nfc", "Failed to create folder %s", furi_string_get_cstr(folder));
            break;
        }
        furi_string_free(folder);
        // First remove nfc device file if it was saved
        // Open file
        if(!flipper_format_file_open_always(file, furi_string_get_cstr(temp_str))) break;
        // Write header
        if(!flipper_format_write_header_cstr(file, nfc_file_header, nfc_file_version)) break;
        // Write nfc device type
        if(!flipper_format_write_comment_cstr(
               file, "Nfc device type can be UID, Mifare Ultralight, Mifare Classic or ISO15693"))
            break;
        nfc_device_prepare_format_string(dev, temp_str);
        if(!flipper_format_write_string(file, "Device type", temp_str)) break;
        // Write UID
        if(!flipper_format_write_comment_cstr(file, "UID is common for all formats")) break;
        if(!flipper_format_write_hex(file, "UID", data->uid, data->uid_len)) break;

        if(dev->format != NfcDeviceSaveFormatNfcV) {
            // Write ATQA, SAK
            if(!flipper_format_write_comment_cstr(file, "ISO14443 specific fields")) break;
            // Save ATQA in MSB order for correct companion apps display
            uint8_t atqa[2] = {data->atqa[1], data->atqa[0]};
            if(!flipper_format_write_hex(file, "ATQA", atqa, 2)) break;
            if(!flipper_format_write_hex(file, "SAK", &data->sak, 1)) break;
        }

        // Save more data if necessary
        if(dev->format == NfcDeviceSaveFormatMifareUl) {
            if(!nfc_device_save_mifare_ul_data(file, dev)) break;
        } else if(dev->format == NfcDeviceSaveFormatMifareClassic) {
            // Save data
            if(!nfc_device_save_mifare_classic_data(file, dev)) break;
            // Save keys cache
            if(!nfc_device_save_mifare_classic_keys(dev)) break;
        }
        saved = true;
    } while(0);

    if(!saved) { //-V547
        dialog_message_show_storage_error(dev->dialogs, "Can not save\nkey file");
    }
    furi_string_free(temp_str);
    flipper_format_free(file);
    return saved;
}

bool nfc_device_save_shadow(NfcDevice* dev, const char* path) {
    return false;
    dev->shadow_file_exist = true;
    // Replace extension from .nfc to .shd if necessary
    FuriString* orig_path = furi_string_alloc();
    furi_string_set_str(orig_path, path);
    FuriString* shadow_path = furi_string_alloc();
    nfc_device_get_shadow_path(orig_path, shadow_path);

    bool file_saved = nfc_device_save(dev, furi_string_get_cstr(shadow_path));
    furi_string_free(orig_path);
    furi_string_free(shadow_path);

    return file_saved;
}

static bool nfc_device_load_data(NfcDevice* dev, FuriString* path, bool show_dialog) {
    bool parsed = false;
    FlipperFormat* file = flipper_format_file_alloc(dev->storage);
    FurryHalNfcDevData* data = &dev->dev_data.nfc_data;
    uint32_t data_cnt = 0;
    FuriString* temp_str;
    temp_str = furi_string_alloc();
    bool deprecated_version = false;

    // Version 2 of file format had ATQA bytes swapped
    uint32_t version_with_lsb_atqa = 2;

    if(dev->loading_cb) {
        dev->loading_cb(dev->loading_cb_ctx, true);
    }

    do {
        // Check existence of shadow file
        nfc_device_get_shadow_path(path, temp_str);
        dev->shadow_file_exist =
            storage_common_stat(dev->storage, furi_string_get_cstr(temp_str), NULL) == FSE_OK;
        // Open shadow file if it exists. If not - open original
        if(dev->shadow_file_exist) {
            if(!flipper_format_file_open_existing(file, furi_string_get_cstr(temp_str))) break;
        } else {
            if(!flipper_format_file_open_existing(file, furi_string_get_cstr(path))) break;
        }
        // Read and verify file header
        uint32_t version = 0;
        if(!flipper_format_read_header(file, temp_str, &version)) break;
        if(furi_string_cmp_str(temp_str, nfc_file_header)) break;
        if(version != nfc_file_version) {
            if(version < version_with_lsb_atqa) {
                deprecated_version = true;
                break;
            }
        }
        // Read Nfc device type
        if(!flipper_format_read_string(file, "Device type", temp_str)) break;
        if(!nfc_device_parse_format_string(dev, temp_str)) break;
        // Read and parse UID, ATQA and SAK
        if(!flipper_format_get_value_count(file, "UID", &data_cnt)) break;
        if(!(data_cnt == 4 || data_cnt == 7 || data_cnt == 8)) break;
        data->uid_len = data_cnt;
        if(!flipper_format_read_hex(file, "UID", data->uid, data->uid_len)) break;
        if(dev->format != NfcDeviceSaveFormatNfcV) {
            if(version == version_with_lsb_atqa) {
                if(!flipper_format_read_hex(file, "ATQA", data->atqa, 2)) break;
            } else {
                uint8_t atqa[2] = {};
                if(!flipper_format_read_hex(file, "ATQA", atqa, 2)) break;
                data->atqa[0] = atqa[1];
                data->atqa[1] = atqa[0];
            }
            if(!flipper_format_read_hex(file, "SAK", &data->sak, 1)) break;
        }
        // Load CUID
        uint8_t* cuid_start = data->uid;
        if(data->uid_len == 7) {
            cuid_start = &data->uid[3];
        }
        data->cuid = (cuid_start[0] << 24) | (cuid_start[1] << 16) | (cuid_start[2] << 8) |
                     (cuid_start[3]);
        // Parse other data
        if(dev->format == NfcDeviceSaveFormatMifareUl) {
            if(!nfc_device_load_mifare_ul_data(file, dev)) break;
        } else if(dev->format == NfcDeviceSaveFormatMifareClassic) {
            if(!nfc_device_load_mifare_classic_data(file, dev)) break;
        }
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

bool nfc_device_load(NfcDevice* dev, const char* file_path, bool show_dialog) {
    furi_assert(dev);
    furi_assert(file_path);

    // Load device data
    furi_string_set(dev->load_path, file_path);
    bool dev_load = nfc_device_load_data(dev, dev->load_path, show_dialog);
    if(dev_load) {
        // Set device name
        FuriString* filename;
        filename = furi_string_alloc();
        path_extract_filename_no_ext(file_path, filename);
        nfc_device_set_name(dev, furi_string_get_cstr(filename));
        furi_string_free(filename);
    }

    return dev_load;
}

void nfc_device_data_clear(NfcDeviceData* dev_data) {
    if(dev_data->protocol == NfcDeviceProtocolMifareClassic) {
        memset(&dev_data->mf_classic_data, 0, sizeof(MfClassicData));
    } else if(dev_data->protocol == NfcDeviceProtocolMifareUl) {
        mf_ul_reset(&dev_data->mf_ul_data);
    }

    memset(&dev_data->nfc_data, 0, sizeof(FurryHalNfcDevData));
    dev_data->protocol = NfcDeviceProtocolUnknown;
    if(dev_data->parsed_data != NULL) {
        furi_string_reset(dev_data->parsed_data);
    }
}

void nfc_device_clear(NfcDevice* dev) {
    furi_assert(dev);

    nfc_device_set_name(dev, "");
    nfc_device_data_clear(&dev->dev_data);
    dev->format = NfcDeviceSaveFormatUid;
    furi_string_reset(dev->load_path);
}

bool nfc_device_delete(NfcDevice* dev, bool use_load_path) {
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
                file_path,
                "%s/%s%s",
                furi_string_get_cstr(dev->folder),
                dev->dev_name,
                NFC_APP_FILENAME_EXTENSION);
        }
        if(!storage_simply_remove(dev->storage, furi_string_get_cstr(file_path))) break;
        // Delete shadow file if it exists
        if(dev->shadow_file_exist) {
            if(use_load_path && !furi_string_empty(dev->load_path)) {
                nfc_device_get_shadow_path(dev->load_path, file_path);
            } else {
                furi_string_printf(
                    file_path,
                    "%s/%s%s",
                    furi_string_get_cstr(dev->folder),
                    dev->dev_name,
                    NFC_APP_SHADOW_EXTENSION);
            }
            if(!storage_simply_remove(dev->storage, furi_string_get_cstr(file_path))) break;
        }
        deleted = true;
    } while(0);

    if(!deleted) {
        dialog_message_show_storage_error(dev->dialogs, "Can not remove file");
    }

    furi_string_free(file_path);
    return deleted;
}

bool nfc_device_restore(NfcDevice* dev, bool use_load_path) {
    furi_assert(dev);
    furi_assert(dev->shadow_file_exist);

    bool restored = false;
    FuriString* path;

    path = furi_string_alloc();

    do {
        if(use_load_path && !furi_string_empty(dev->load_path)) {
            nfc_device_get_shadow_path(dev->load_path, path);
        } else {
            furi_string_printf(
                path,
                "%s/%s%s",
                furi_string_get_cstr(dev->folder),
                dev->dev_name,
                NFC_APP_SHADOW_EXTENSION);
        }
        if(!storage_simply_remove(dev->storage, furi_string_get_cstr(path))) break;
        dev->shadow_file_exist = false;
        if(use_load_path && !furi_string_empty(dev->load_path)) {
            furi_string_set(path, dev->load_path);
        } else {
            furi_string_printf(
                path,
                "%s/%s%s",
                furi_string_get_cstr(dev->folder),
                dev->dev_name,
                NFC_APP_FILENAME_EXTENSION);
        }
        if(!nfc_device_load_data(dev, path, true)) break;
        restored = true;
    } while(0);

    furi_string_free(path);
    return restored;
}

void nfc_device_set_loading_callback(NfcDevice* dev, NfcLoadingCallback callback, void* context) {
    furi_assert(dev);

    dev->loading_cb = callback;
    dev->loading_cb_ctx = context;
}
