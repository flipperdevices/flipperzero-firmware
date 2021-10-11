#include "nfc_device_i.h"

#include <lib/toolbox/path.h>
#include <lib/toolbox/hex.h>
#include <lib/toolbox/flipper-file.h>

// TODO remove file worker
#include <file-worker.h>

static const char* nfc_app_folder = "/any/nfc";
static const char* nfc_app_extension = ".nfc";
static const char* nfc_app_shadow_extension = ".shd";
static const char* nfc_file_header = "Flipper NFC device";
static const uint32_t nfc_file_version = 1;

NfcDevice* nfc_device_alloc() {
    NfcDevice* nfc_dev = furi_alloc(sizeof(NfcDevice));
    nfc_dev->storage = furi_record_open("storage");
    nfc_dev->dialogs = furi_record_open("dialogs");
    return nfc_dev;
}

void nfc_device_free(NfcDevice* nfc_dev) {
    furi_assert(nfc_dev);
    furi_record_close("storage");
    furi_record_close("dialogs");
    free(nfc_dev);
}

static bool nfc_device_read_hex(string_t str, uint8_t* buff, uint16_t len, uint8_t delim_len) {
    string_strim(str);
    uint8_t nibble_high = 0;
    uint8_t nibble_low = 0;
    bool parsed = true;

    for(uint16_t i = 0; i < len; i++) {
        if(hex_char_to_hex_nibble(string_get_char(str, 0), &nibble_high) &&
           hex_char_to_hex_nibble(string_get_char(str, 1), &nibble_low)) {
            buff[i] = (nibble_high << 4) | nibble_low;
            string_right(str, delim_len + 2);
        } else {
            parsed = false;
            break;
        }
    }
    return parsed;
}

void nfc_device_prepare_format_string(NfcDevice* dev, string_t format_string) {
    if(dev->format == NfcDeviceSaveFormatUid) {
        string_set_str(format_string, "UID\n");
    } else if(dev->format == NfcDeviceSaveFormatBankCard) {
        string_set_str(format_string, "Bank card\n");
    } else if(dev->format == NfcDeviceSaveFormatMifareUl) {
        string_set_str(format_string, "Mifare Ultralight\n");
    } else {
        string_set_str(format_string, "Unknown\n");
    }
}

bool nfc_device_parse_format_string(NfcDevice* dev, string_t format_string) {
    if(string_start_with_str_p(format_string, "UID")) {
        dev->format = NfcDeviceSaveFormatUid;
        dev->dev_data.nfc_data.protocol = NfcDeviceProtocolUnknown;
        return true;
    } else if(string_start_with_str_p(format_string, "Bank card")) {
        dev->format = NfcDeviceSaveFormatBankCard;
        dev->dev_data.nfc_data.protocol = NfcDeviceProtocolEMV;
        return true;
    } else if(string_start_with_str_p(format_string, "Mifare Ultralight")) {
        dev->format = NfcDeviceSaveFormatMifareUl;
        dev->dev_data.nfc_data.protocol = NfcDeviceProtocolMifareUl;
        return true;
    }
    return false;
}

bool nfc_device_parse_uid_string(NfcDevice* dev, string_t uid_string) {
    NfcDeviceCommonData* uid_data = &dev->dev_data.nfc_data;
    bool parsed = false;

    do {
        // strlen("UID len: ") = 9
        string_right(uid_string, 9);
        if(!nfc_device_read_hex(uid_string, &uid_data->uid_len, 1, 1)) {
            break;
        }
        // strlen("UID: ") = 5
        string_right(uid_string, 5);
        if(!nfc_device_read_hex(uid_string, uid_data->uid, uid_data->uid_len, 1)) {
            break;
        }
        // strlen("ATQA: ") = 6
        string_right(uid_string, 6);
        if(!nfc_device_read_hex(uid_string, uid_data->atqa, 2, 1)) {
            break;
        }
        // strlen("SAK: ") = 5
        string_right(uid_string, 5);
        if(!nfc_device_read_hex(uid_string, &uid_data->sak, 1, 1)) {
            break;
        }
        parsed = true;
    } while(0);

    return parsed;
}

static bool nfc_device_save_mifare_ul_data(FlipperFile* file, NfcDevice* dev) {
    bool saved = false;
    MifareUlData* data = &dev->dev_data.mf_ul_data;
    string_t temp_str;
    string_init(temp_str);

    // Save Mifare Ultralight specific data
    do {
        if(!flipper_file_write_comment_cstr(file, "Mifare Ultralight specific data")) break;
        if(!flipper_file_write_hex_array(
               file, "Signature", data->signature, sizeof(data->signature)))
            break;
        if(!flipper_file_write_hex_array(
               file, "Mifare version", (uint8_t*)&data->version, sizeof(data->version)))
            break;
        // Write conters and tearing flags data
        bool counters_saved = true;
        for(uint8_t i = 0; i < 3; i++) {
            string_printf(temp_str, "Counter %d", i);
            if(!flipper_file_write_uint32(file, string_get_cstr(temp_str), data->counter[i])) {
                counters_saved = false;
                break;
            }
            string_printf(temp_str, "Tearing %d", i);
            if(!flipper_file_write_hex_array(
                   file, string_get_cstr(temp_str), &data->tearing[i], 1)) {
                counters_saved = false;
                break;
            }
        }
        if(!counters_saved) break;
        // Write pages data
        bool pages_saved = true;
        for(uint16_t i = 0; i < data->data_size; i += 4) {
            string_printf(temp_str, "Page %d", i / 4);
            if(!flipper_file_write_hex_array(file, string_get_cstr(temp_str), &data->data[i], 4)) {
                pages_saved = false;
                break;
            }
        }
        if(!pages_saved) break;
        saved = true;
    } while(false);

    string_clear(temp_str);
    return saved;
}

bool nfc_device_parse_mifare_ul_string(NfcDevice* dev, string_t mifare_ul_string) {
    MifareUlData* data = &dev->dev_data.mf_ul_data;
    uint16_t tearing_tmp = 0;
    uint16_t cnt_num = 0;
    size_t ws = 0;
    int res = 0;
    bool parsed = false;

    do {
        // strlen("Signature: ") = 11
        string_right(mifare_ul_string, 11);
        if(!nfc_device_read_hex(mifare_ul_string, data->signature, sizeof(data->signature), 1)) {
            break;
        }
        // strlen("Version: ") = 9
        string_right(mifare_ul_string, 9);
        if(!nfc_device_read_hex(
               mifare_ul_string, (uint8_t*)&data->version, sizeof(data->version), 1)) {
            break;
        }
        string_strim(mifare_ul_string);
        // Read counters and tearing flags
        for(uint8_t i = 0; i < 3; i++) {
            res = sscanf(
                string_get_cstr(mifare_ul_string),
                "Counter %hX: %lu Tearing flag %hX: %02hX",
                &cnt_num,
                &data->counter[i],
                &cnt_num,
                &tearing_tmp);
            if(res != 4) {
                break;
            }
            data->tearing[i] = tearing_tmp;
            ws = string_search_char(mifare_ul_string, '\n');
            string_right(mifare_ul_string, ws + 1);
        }
        // Read data size
        res = sscanf(string_get_cstr(mifare_ul_string), "Data size: %hu", &data->data_size);
        if(res != 1) {
            break;
        }
        ws = string_search_char(mifare_ul_string, '\n');
        string_right(mifare_ul_string, ws + 1);
        // Read data
        for(uint16_t i = 0; i < data->data_size; i += 4) {
            if(!nfc_device_read_hex(mifare_ul_string, &data->data[i], 4, 1)) {
                break;
            }
        }
        parsed = true;
    } while(0);

    return parsed;
}

static bool nfc_device_save_bank_card_data(FlipperFile* file, NfcDevice* dev) {
    bool saved = false;
    NfcEmvData* data = &dev->dev_data.emv_data;

    do {
        // Write Bank card specific data
        if(!flipper_file_write_comment_cstr(file, "Bank card specific data")) break;
        if(!flipper_file_write_hex_array(file, "AID", data->aid, data->aid_len)) break;
        if(!flipper_file_write_string_cstr(file, "Name", data->name)) break;
        if(!flipper_file_write_hex_array(file, "Number", data->number, data->number_len)) break;
        if(data->exp_mon) {
            uint8_t exp_data[2] = {data->exp_mon, data->exp_mon};
            if(!flipper_file_write_hex_array(file, "Exp data", exp_data, sizeof(exp_data))) break;
        }
        if(data->country_code) {
            if(!flipper_file_write_uint32(file, "Country code", data->country_code)) break;
        }
        if(data->currency_code) {
            if(!flipper_file_write_uint32(file, "Currency code", data->currency_code)) break;
        }
        saved = true;
    } while(false);

    return saved;
}

bool nfc_device_parse_bank_card_string(NfcDevice* dev, string_t bank_card_string) {
    NfcEmvData* data = &dev->dev_data.emv_data;
    bool parsed = false;
    int res = 0;
    uint8_t code[2] = {};
    memset(data, 0, sizeof(NfcEmvData));

    do {
        res = sscanf(string_get_cstr(bank_card_string), "AID len: %hu", &data->aid_len);
        if(res != 1) {
            break;
        }
        // strlen("AID len: ") = 9
        string_right(bank_card_string, 9);
        size_t ws = string_search_char(bank_card_string, ':');
        string_right(bank_card_string, ws + 1);
        if(!nfc_device_read_hex(bank_card_string, data->aid, data->aid_len, 1)) {
            break;
        }
        res = sscanf(string_get_cstr(bank_card_string), "Name: %s\n", data->name);
        if(res != 1) {
            break;
        }
        ws = string_search_char(bank_card_string, '\n');
        string_right(bank_card_string, ws + 1);
        res = sscanf(string_get_cstr(bank_card_string), "Number len: %hhu", &data->number_len);
        if(res != 1) {
            break;
        }
        ws = string_search_char(bank_card_string, '\n');
        string_right(bank_card_string, ws + 1);
        // strlen("Number: ") = 8
        string_right(bank_card_string, 8);
        if(!nfc_device_read_hex(bank_card_string, data->number, data->number_len, 1)) {
            break;
        }
        parsed = true;
        // Check expiration date presence
        ws = string_search_str(bank_card_string, "Exp date: ");
        if(ws != STRING_FAILURE) {
            // strlen("Exp date: ") = 10
            string_right(bank_card_string, 10);
            nfc_device_read_hex(bank_card_string, &data->exp_mon, 1, 1);
            nfc_device_read_hex(bank_card_string, &data->exp_year, 1, 1);
        }
        // Check country code presence
        ws = string_search_str(bank_card_string, "Country code: ");
        if(ws != STRING_FAILURE) {
            // strlen("Country code: ") = 14
            string_right(bank_card_string, 14);
            nfc_device_read_hex(bank_card_string, code, 2, 0);
            data->country_code = code[0] << 8 | code[1];
        }
        // Check currency code presence
        ws = string_search_str(bank_card_string, "Currency code: ");
        if(ws != STRING_FAILURE) {
            // strlen("Currency code: ") = 15
            string_right(bank_card_string, 15);
            nfc_device_read_hex(bank_card_string, code, 2, 0);
            data->currency_code = code[0] << 8 | code[1];
        }
    } while(0);

    return parsed;
}

void nfc_device_set_name(NfcDevice* dev, const char* name) {
    furi_assert(dev);

    strlcpy(dev->dev_name, name, NFC_DEV_NAME_MAX_LEN);
}

static bool nfc_device_save_file(
    NfcDevice* dev,
    const char* dev_name,
    const char* folder,
    const char* extension) {
    furi_assert(dev);

    bool saved = false;
    FlipperFile* file = flipper_file_alloc(dev->storage);
    string_t temp_str;
    string_init(temp_str);

    do {
        // Create nfc directory if necessary
        if(!storage_simply_mkdir(dev->storage, nfc_app_folder)) break;
        // First remove nfc device file if it was saved
        string_printf(temp_str, "%s/%s%s", folder, dev_name, extension);
        if(!storage_simply_remove(dev->storage, string_get_cstr(temp_str))) break;
        // Open file
        if(!flipper_file_new_write(file, string_get_cstr(temp_str))) break;
        // Write header
        if(!flipper_file_write_header_cstr(file, nfc_file_header, nfc_file_version)) break;
        // Write nfc device type
        if(!flipper_file_write_comment_cstr(
               file, "Nfc device type can be UID, Mifare Ultralight, Bank card"))
            break;
        nfc_device_prepare_format_string(dev, temp_str);
        if(!flipper_file_write_string(file, "Device type", temp_str)) break;
        // Write UID, ATQA, SAK
        if(!flipper_file_write_comment_cstr(file, "UID, ATQA and SAK are common for all formats"))
            break;
        if(!flipper_file_write_hex_array(
               file, "UID", dev->dev_data.nfc_data.uid, dev->dev_data.nfc_data.uid_len))
            break;
        if(!flipper_file_write_hex_array(file, "ATQA", dev->dev_data.nfc_data.atqa, 2)) break;
        if(!flipper_file_write_hex_array(file, "SAK", &dev->dev_data.nfc_data.sak, 1)) break;
        // Save more data if necessary
        if(dev->format == NfcDeviceSaveFormatMifareUl) {
            if(!nfc_device_save_mifare_ul_data(file, dev)) break;
        } else if(dev->format == NfcDeviceSaveFormatBankCard) {
            if(!nfc_device_save_bank_card_data(file, dev)) break;
        }
        saved = true;
    } while(0);

    if(!saved) {
        dialog_message_show_storage_error(dev->dialogs, "Can not save\nkey file");
    }
    string_clear(temp_str);
    flipper_file_close(file);
    flipper_file_free(file);
    return saved;
}

bool nfc_device_save(NfcDevice* dev, const char* dev_name) {
    return nfc_device_save_file(dev, dev_name, nfc_app_folder, nfc_app_extension);
}

bool nfc_device_save_shadow(NfcDevice* dev, const char* dev_name) {
    dev->shadow_file_exist = true;
    return nfc_device_save_file(dev, dev_name, nfc_app_folder, nfc_app_shadow_extension);
}

static bool nfc_device_load_data(FileWorker* file_worker, string_t path, NfcDevice* dev) {
    string_t temp_string;
    string_init(temp_string);
    bool parsed = false;

    do {
        // Check existance of shadow file
        size_t ext_start = string_search_str(path, nfc_app_extension);
        string_set_n(temp_string, path, 0, ext_start);
        string_cat_printf(temp_string, "%s", nfc_app_shadow_extension);
        if(!file_worker_is_file_exist(
               file_worker, string_get_cstr(temp_string), &dev->shadow_file_exist)) {
            break;
        }
        // Open shadow file if it exists. If not - open original
        if(dev->shadow_file_exist) {
            if(!file_worker_open(
                   file_worker, string_get_cstr(temp_string), FSAM_READ, FSOM_OPEN_EXISTING)) {
                break;
            }
        } else {
            if(!file_worker_open(
                   file_worker, string_get_cstr(path), FSAM_READ, FSOM_OPEN_EXISTING)) {
                break;
            }
        }

        // Read and parse format from 1st line
        if(!file_worker_read_until(file_worker, temp_string, '\n')) {
            break;
        }
        if(!nfc_device_parse_format_string(dev, temp_string)) {
            break;
        }
        // Read and parse UID data from 2nd line
        if(!file_worker_read_until(file_worker, temp_string, '\n')) {
            break;
        }
        if(!nfc_device_parse_uid_string(dev, temp_string)) {
            break;
        }
        // Parse other data
        if(dev->format == NfcDeviceSaveFormatMifareUl) {
            // Read until EOF
            if(!file_worker_read_until(file_worker, temp_string, 0x05)) {
                break;
            }
            if(!nfc_device_parse_mifare_ul_string(dev, temp_string)) {
                break;
            }
        } else if(dev->format == NfcDeviceSaveFormatBankCard) {
            // Read until EOF
            if(!file_worker_read_until(file_worker, temp_string, 0x05)) {
                break;
            }
            if(!nfc_device_parse_bank_card_string(dev, temp_string)) {
                break;
            }
        }
        parsed = true;
    } while(0);

    if(!parsed) {
        file_worker_show_error(file_worker, "Can not parse\nfile");
    }

    string_clear(temp_string);
    return parsed;
}

bool nfc_device_load(NfcDevice* dev, const char* file_path) {
    furi_assert(dev);
    furi_assert(file_path);

    FileWorker* file_worker = file_worker_alloc(false);
    // Load device data
    string_t path;
    string_init_set_str(path, file_path);
    bool dev_load = nfc_device_load_data(file_worker, path, dev);
    if(dev_load) {
        // Set device name
        path_extract_filename_no_ext(file_path, path);
        nfc_device_set_name(dev, string_get_cstr(path));
    }
    string_clear(path);
    file_worker_close(file_worker);
    file_worker_free(file_worker);

    return dev_load;
}

bool nfc_file_select(NfcDevice* dev) {
    furi_assert(dev);

    FileWorker* file_worker = file_worker_alloc(false);
    // Input events and views are managed by file_select
    bool res = file_worker_file_select(
        file_worker,
        nfc_app_folder,
        nfc_app_extension,
        dev->file_name,
        sizeof(dev->file_name),
        dev->dev_name);
    if(res) {
        string_t dev_str;

        // Get key file path
        string_init_printf(dev_str, "%s/%s%s", nfc_app_folder, dev->file_name, nfc_app_extension);

        res = nfc_device_load_data(file_worker, dev_str, dev);
        if(res) {
            nfc_device_set_name(dev, dev->file_name);
        }
        string_clear(dev_str);
    }
    file_worker_close(file_worker);
    file_worker_free(file_worker);

    return res;
}

void nfc_device_clear(NfcDevice* dev) {
    furi_assert(dev);

    memset(&dev->dev_data, 0, sizeof(dev->dev_data));
    dev->format = NfcDeviceSaveFormatUid;
}

bool nfc_device_delete(NfcDevice* dev) {
    furi_assert(dev);

    bool result = true;
    FileWorker* file_worker = file_worker_alloc(false);
    string_t file_path;

    do {
        // Delete original file
        string_init_printf(file_path, "%s/%s%s", nfc_app_folder, dev->dev_name, nfc_app_extension);
        if(!file_worker_remove(file_worker, string_get_cstr(file_path))) {
            result = false;
            break;
        }
        // Delete shadow file if it exists
        if(dev->shadow_file_exist) {
            string_clean(file_path);
            string_printf(
                file_path, "%s/%s%s", nfc_app_folder, dev->dev_name, nfc_app_shadow_extension);
            if(!file_worker_remove(file_worker, string_get_cstr(file_path))) {
                result = false;
                break;
            }
        }
    } while(0);

    string_clear(file_path);
    file_worker_close(file_worker);
    file_worker_free(file_worker);
    return result;
}

bool nfc_device_restore(NfcDevice* dev) {
    furi_assert(dev);
    furi_assert(dev->shadow_file_exist);

    bool result = true;
    FileWorker* file_worker = file_worker_alloc(false);
    string_t path;

    do {
        string_init_printf(
            path, "%s/%s%s", nfc_app_folder, dev->dev_name, nfc_app_shadow_extension);
        if(!file_worker_remove(file_worker, string_get_cstr(path))) {
            result = false;
            break;
        }
        dev->shadow_file_exist = false;
        string_clean(path);
        string_printf(path, "%s/%s%s", nfc_app_folder, dev->dev_name, nfc_app_extension);
        if(!nfc_device_load_data(file_worker, path, dev)) {
            result = false;
            break;
        }
    } while(0);

    string_clear(path);
    file_worker_close(file_worker);
    file_worker_free(file_worker);
    return result;
}
