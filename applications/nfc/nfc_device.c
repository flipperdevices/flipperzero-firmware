#include "nfc_device_i.h"

#include <file-worker.h>
#include <path.h>
#include <hex.h>

#define NFC_DEVICE_MAX_DATA_LEN 14

static const char* nfc_app_folder = "nfc";
static const char* nfc_app_extension = ".nfc";

uint16_t nfc_device_prepare_format_string(NfcDevice* dev, string_t format_string) {
    if(dev->format == NfcDeviceSaveFormatUid) {
        string_set_str(format_string, "UID\n");
    } else if(dev->format == NfcDeviceSaveFormatBankCard) {
        string_set_str(format_string, "Bank card\n");
    } else if(dev->format == NfcDeviceSaveFormatMifareUl) {
        string_set_str(format_string, "Mifare Ultralight\n");
    } else {
        string_set_str(format_string, "Unknown\n");
    }
    return string_size(format_string);
}

bool nfc_device_parse_format_string(NfcDevice* dev, string_t format_string) {
    if(string_start_with_str_p(format_string, "UID")) {
        dev->format = NfcDeviceSaveFormatUid;
        return true;
    } else if(string_start_with_str_p(format_string, "Bank card")) {
        dev->format = NfcDeviceSaveFormatBankCard;
        return true;
    } else if(string_start_with_str_p(format_string, "Mifare Ultralight")) {
        dev->format = NfcDeviceSaveFormatMifareUl;
        return true;
    }
    return false;
}

uint16_t nfc_device_prepare_uid_string(NfcDevice* dev, string_t uid_string) {
    NfcDeviceCommomData* uid_data = &dev->dev_data.nfc_data;
    string_printf(uid_string, "UID len: %02X UID: ", dev->dev_data.nfc_data.uid_len);
    for(uint8_t i = 0; i < uid_data->uid_len; i++) {
        string_cat_printf(uid_string, "%02X ", uid_data->uid[i]);
    }
    string_cat_printf(
        uid_string,
        "ATQA: %02X %02X SAK: %02X\n",
        uid_data->atqa[0],
        uid_data->atqa[1],
        uid_data->sak);
    return string_size(uid_string);
}

bool nfc_device_parse_uid_string(NfcDevice* dev, string_t uid_string) {
    NfcDeviceCommomData* uid_data = &dev->dev_data.nfc_data;
    int parsed = 0;
    // Sscanf only works with u16
    uint16_t data_u16[14] = {};

    parsed = sscanf(string_get_cstr(uid_string), "UID len: %02hhX ", &uid_data->uid_len);
    if(parsed != 1) {
        return false;
    }
    if(uid_data->uid_len == 4) {
        parsed = sscanf(
            string_get_cstr(uid_string),
            "UID len: %02hX UID: %02hX %02hX %02hX %02hX ATQA: %02hX %02hX SAK: %02hX\n",
            &data_u16[0],
            &data_u16[1],
            &data_u16[2],
            &data_u16[3],
            &data_u16[4],
            &data_u16[5],
            &data_u16[6],
            &data_u16[7]);

        uid_data->uid[0] = (uint8_t)data_u16[1];
        uid_data->uid[1] = (uint8_t)data_u16[2];
        uid_data->uid[2] = (uint8_t)data_u16[3];
        uid_data->uid[3] = (uint8_t)data_u16[4];
        uid_data->atqa[0] = (uint8_t)data_u16[5];
        uid_data->atqa[1] = (uint8_t)data_u16[6];
        uid_data->sak = (uint8_t)data_u16[7];
        if(parsed == 8) {
            return true;
        }
    } else if(uid_data->uid_len == 7) {
        parsed = sscanf(
            string_get_cstr(uid_string),
            "UID len: %02hX UID: %02hX %02hX %02hX %02hX %02hX %02hX %02hX ATQA: %02hX %02hX SAK: %02hX\n",
            &data_u16[0],
            &data_u16[1],
            &data_u16[2],
            &data_u16[3],
            &data_u16[4],
            &data_u16[5],
            &data_u16[6],
            &data_u16[7],
            &data_u16[8],
            &data_u16[9],
            &data_u16[10]);

        uid_data->uid[0] = (uint8_t)data_u16[1];
        uid_data->uid[1] = (uint8_t)data_u16[2];
        uid_data->uid[2] = (uint8_t)data_u16[3];
        uid_data->uid[3] = (uint8_t)data_u16[4];
        uid_data->uid[4] = (uint8_t)data_u16[5];
        uid_data->uid[5] = (uint8_t)data_u16[6];
        uid_data->uid[6] = (uint8_t)data_u16[7];
        uid_data->atqa[0] = (uint8_t)data_u16[8];
        uid_data->atqa[1] = (uint8_t)data_u16[9];
        uid_data->sak = (uint8_t)data_u16[10];
        if(parsed == 11) {
            return true;
        }
    } else if(uid_data->uid_len == 10) {
        parsed = sscanf(
            string_get_cstr(uid_string),
            "UID len: %02hX UID: %02hX %02hX %02hX %02hX %02hX %02hX %02hX %02hX %02hX %02hX ATQA: %02hX %02hX SAK: %02hX\n",
            &data_u16[0],
            &data_u16[1],
            &data_u16[2],
            &data_u16[3],
            &data_u16[4],
            &data_u16[5],
            &data_u16[6],
            &data_u16[7],
            &data_u16[8],
            &data_u16[9],
            &data_u16[10],
            &data_u16[11],
            &data_u16[12],
            &data_u16[13]);

        uid_data->uid[0] = (uint8_t)data_u16[1];
        uid_data->uid[1] = (uint8_t)data_u16[2];
        uid_data->uid[2] = (uint8_t)data_u16[3];
        uid_data->uid[3] = (uint8_t)data_u16[4];
        uid_data->uid[4] = (uint8_t)data_u16[5];
        uid_data->uid[5] = (uint8_t)data_u16[6];
        uid_data->uid[6] = (uint8_t)data_u16[7];
        uid_data->uid[7] = (uint8_t)data_u16[8];
        uid_data->uid[8] = (uint8_t)data_u16[9];
        uid_data->uid[9] = (uint8_t)data_u16[10];
        uid_data->atqa[0] = (uint8_t)data_u16[11];
        uid_data->atqa[1] = (uint8_t)data_u16[12];
        uid_data->sak = (uint8_t)data_u16[13];
        if(parsed == 14) {
            return true;
        }
    }
    return false;
}

uint16_t nfc_device_prepare_mifare_ul_string(NfcDevice* dev, string_t mifare_ul_string) {
    MifareUlData* data = &dev->dev_data.mf_ul_data;
    string_printf(mifare_ul_string, "Signature:");
    for(uint8_t i = 0; i < sizeof(data->signature); i++) {
        string_cat_printf(mifare_ul_string, " %02X", data->signature[i]);
    }
    string_cat_printf(mifare_ul_string, "\nVersion:");
    uint8_t* version = (uint8_t*)&data->version;
    for(uint8_t i = 0; i < sizeof(data->version); i++) {
        string_cat_printf(mifare_ul_string, " %02X", version[i]);
    }
    for(uint8_t i = 0; i < 3; i++) {
        string_cat_printf(
            mifare_ul_string,
            "\nCounter %d: %lu Tearing flag %d: %02X",
            i,
            data->counter[i],
            i,
            data->tearing[i]);
    }
    string_cat_printf(mifare_ul_string, "\nData size: %d\n", data->data_size);
    for(uint16_t i = 0; i < data->data_size; i += 4) {
        string_cat_printf(
            mifare_ul_string,
            "%02X %02X %02X %02X\n",
            data->data[i],
            data->data[i + 1],
            data->data[i + 2],
            data->data[i + 3]);
    }
    return string_size(mifare_ul_string);
}

bool nfc_device_read_hex(string_t str, uint8_t* buff, uint16_t len) {
    string_strim(str);
    uint8_t nibble_high = 0;
    uint8_t nibble_low = 0;
    bool parsed = true;

    for(uint16_t i = 0; i < len; i++) {
        if(hex_char_to_hex_nibble(string_get_char(str, 0), &nibble_high) &&
        hex_char_to_hex_nibble(string_get_char(str, 1), &nibble_low)) {
            buff[i] = (nibble_high << 4) | nibble_low;
            string_right(str, 3);
        } else {
            parsed = false;
            break;
        }
    }
    return parsed;
}

bool nfc_device_parse_mifare_ul_string(NfcDevice* dev, string_t mifare_ul_string) {
    MifareUlData* data = &dev->dev_data.mf_ul_data;
    uint16_t tearing_tmp = 0;
    size_t ws = 0;

    do {
        string_right(mifare_ul_string, strlen("Signature: "));
        if(!nfc_device_read_hex(mifare_ul_string, data->signature, sizeof(data->signature))) {
            break;
        }
        string_right(mifare_ul_string, strlen("\nVersion: "));
        if(!nfc_device_read_hex(mifare_ul_string, (uint8_t*)&data->version, sizeof(data->version))) {
            break;
        }
    //     string_strim(mifare_ul_string);
    //     sscanf(string_get_cstr(mifare_ul_string), "Counter 0: %u Tearing flag 0: %02hX", data->counter[0], &tearing_tmp);
    //     data->tearing[0] = tearing_tmp;
    //     ws = string_search_char(mifare_ul_string, '\n');
    //     string_right(mifare_ul_string, ws + 1);
    //     sscanf(string_get_cstr(mifare_ul_string), "Counter 1: %u Tearing flag 1: %02hX", data->counter[1], &tearing_tmp);
    //     data->tearing[1] = tearing_tmp;
    //     ws = string_search_char(mifare_ul_string, '\n');
    //     string_right(mifare_ul_string, ws + 1);
    //     sscanf(string_get_cstr(mifare_ul_string), "Counter 2: %u Tearing flag 2: %02hX", data->counter[2], &tearing_tmp);
    //     data->tearing[2] = tearing_tmp;
    //     ws = string_search_char(mifare_ul_string, '\n');
    //     string_right(mifare_ul_string, ws + 1);

    // } while(0);

    return true;
}

void nfc_device_set_name(NfcDevice* dev, const char* name) {
    furi_assert(dev);

    strlcpy(dev->dev_name, name, NFC_DEV_NAME_MAX_LEN);
}

bool nfc_device_save(NfcDevice* dev, const char* dev_name) {
    furi_assert(dev);

    FileWorker* file_worker = file_worker_alloc(false);
    string_t dev_file_name;
    string_init(dev_file_name);
    string_t temp_str;
    string_init(temp_str);
    uint16_t string_len = 0;

    do {
        // Create nfc directory if necessary
        if(!file_worker_mkdir(file_worker, nfc_app_folder)) {
            break;
        };
        // First remove nfc device file if it was saved
        string_printf(dev_file_name, "%s/%s%s", nfc_app_folder, dev_name, nfc_app_extension);
        if(!file_worker_remove(file_worker, string_get_cstr(dev_file_name))) {
            break;
        };
        // Open file
        if(!file_worker_open(
               file_worker, string_get_cstr(dev_file_name), FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
            break;
        }
        // Prepare and write format name on 1st line
        string_len = nfc_device_prepare_format_string(dev, temp_str);
        if(!file_worker_write(file_worker, string_get_cstr(temp_str), string_len)) {
            break;
        }
        // Prepare and write UID data on 2nd line
        string_len = nfc_device_prepare_uid_string(dev, temp_str);
        if(!file_worker_write(file_worker, string_get_cstr(temp_str), string_len)) {
            break;
        }
        // Save more data if necessary
        if(dev->format == NfcDeviceSaveFormatMifareUl) {
            string_len = nfc_device_prepare_mifare_ul_string(dev, temp_str);
            if(!file_worker_write(file_worker, string_get_cstr(temp_str), string_len)) {
                break;
            }
        }
    } while(0);

    string_clear(temp_str);
    string_clear(dev_file_name);
    file_worker_close(file_worker);
    file_worker_free(file_worker);

    return true;
}

static bool nfc_device_load_data(FileWorker* file_worker, string_t path, NfcDevice* dev) {
    string_t temp_string;
    string_init(temp_string);
    bool parsed = false;

    do {
        // Open key file
        if(!file_worker_open(file_worker, string_get_cstr(path), FSAM_READ, FSOM_OPEN_EXISTING)) {
            break;
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
        }
        parsed = true;
    } while(0);

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
        NULL);
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
