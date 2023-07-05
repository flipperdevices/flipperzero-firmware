#include "flipbip_file.h"
#include <storage/storage.h>
#include <loader/loader.h>
#include "../helpers/flipbip_string.h"
// From: lib/crypto
#include <memzero.h>
#include <rand.h>

// #define FLIPBIP_APP_BASE_FOLDER APP_DATA_PATH("flipbip")
#define FLIPBIP_APP_BASE_FOLDER EXT_PATH("apps_data/flipbip")
#define FLIPBIP_APP_BASE_FOLDER_PATH(path) FLIPBIP_APP_BASE_FOLDER "/" path
#define FLIPBIP_DAT_FILE_NAME ".flipbip.dat"
// #define FLIPBIP_DAT_FILE_NAME ".flipbip.dat.txt"
#define FLIPBIP_DAT_FILE_NAME_BAK ".flipbip.dat.bak"
#define FLIPBIP_KEY_FILE_NAME ".flipbip.key"
// #define FLIPBIP_KEY_FILE_NAME ".flipbip.key.txt"
#define FLIPBIP_KEY_FILE_NAME_BAK ".flipbip.key.bak"
#define FLIPBIP_DAT_PATH FLIPBIP_APP_BASE_FOLDER_PATH(FLIPBIP_DAT_FILE_NAME)
#define FLIPBIP_DAT_PATH_BAK FLIPBIP_APP_BASE_FOLDER_PATH(FLIPBIP_DAT_FILE_NAME_BAK)
#define FLIPBIP_KEY_PATH FLIPBIP_APP_BASE_FOLDER_PATH(FLIPBIP_KEY_FILE_NAME)
#define FLIPBIP_KEY_PATH_BAK FLIPBIP_APP_BASE_FOLDER_PATH(FLIPBIP_KEY_FILE_NAME_BAK)

const char* TEXT_QRFILE = "Filetype: QRCode\n"
                          "Version: 0\n"
                          "Message: "; // 37 chars + 1 null
#define FILE_HLEN 4
#define FILE_KLEN 256
#define FILE_SLEN 512
#define FILE_MAX_PATH_LEN 48
#define FILE_MAX_QRFILE_CONTENT 90
const char* FILE_HSTR = "fb01";
const char* FILE_K1 = "fb0131d5cf688221c109163908ebe51debb46227c6cc8b37641910833222772a"
                      "baefe6d9ceb651842260e0d1e05e3b90d15e7d5ffaaabc0207bf200a117793a2";

bool flipbip_load_file(char* settings, const FlipBipFile file_type, const char* file_name) {
    bool ret = false;
    const char* path;
    if(file_type == FlipBipFileKey) {
        path = FLIPBIP_KEY_PATH;
    } else if(file_type == FlipBipFileDat) {
        path = FLIPBIP_DAT_PATH;
    } else {
        char path_buf[FILE_MAX_PATH_LEN] = {0};
        strcpy(path_buf, FLIPBIP_APP_BASE_FOLDER); // 22
        strcpy(path_buf + strlen(path_buf), "/");
        strcpy(path_buf + strlen(path_buf), file_name);
        path = path_buf;
    }

    Storage* fs_api = furi_record_open(RECORD_STORAGE);

    File* settings_file = storage_file_alloc(fs_api);
    if(storage_file_open(settings_file, path, FSAM_READ, FSOM_OPEN_EXISTING)) {
        char chr;
        int i = 0;
        while((storage_file_read(settings_file, &chr, 1) == 1) &&
              !storage_file_eof(settings_file) && !isspace(chr)) {
            settings[i] = chr;
            i++;
        }
        ret = true;
    } else {
        memzero(settings, strlen(settings));
        settings[0] = '\0';
        ret = false;
    }
    storage_file_close(settings_file);
    storage_file_free(settings_file);
    furi_record_close(RECORD_STORAGE);

    if(strlen(settings) > 0) {
        Storage* fs_api = furi_record_open(RECORD_STORAGE);
        FileInfo layout_file_info;
        FS_Error file_check_err = storage_common_stat(fs_api, path, &layout_file_info);
        furi_record_close(RECORD_STORAGE);
        if(file_check_err != FSE_OK) {
            memzero(settings, strlen(settings));
            settings[0] = '\0';
            ret = false;
        }
        // if(layout_file_info.size != 256) {
        //     memzero(settings, strlen(settings));
        //     settings[0] = '\0';
        // }
    }

    return ret;
}

bool flipbip_has_file(const FlipBipFile file_type, const char* file_name, const bool remove) {
    bool ret = false;
    const char* path;
    if(file_type == FlipBipFileKey) {
        path = FLIPBIP_KEY_PATH;
    } else if(file_type == FlipBipFileDat) {
        path = FLIPBIP_DAT_PATH;
    } else {
        char path_buf[FILE_MAX_PATH_LEN] = {0};
        strcpy(path_buf, FLIPBIP_APP_BASE_FOLDER); // 22
        strcpy(path_buf + strlen(path_buf), "/");
        strcpy(path_buf + strlen(path_buf), file_name);
        path = path_buf;
    }

    Storage* fs_api = furi_record_open(RECORD_STORAGE);
    if(remove) {
        ret = storage_simply_remove(fs_api, path);
    } else {
        ret = storage_file_exists(fs_api, path);
    }
    furi_record_close(RECORD_STORAGE);

    return ret;
}

bool flipbip_save_file(
    const char* settings,
    const FlipBipFile file_type,
    const char* file_name,
    const bool append) {
    bool ret = false;
    const char* path;
    const char* path_bak;
    if(file_type == FlipBipFileKey) {
        path = FLIPBIP_KEY_PATH;
        path_bak = FLIPBIP_KEY_PATH_BAK;
    } else if(file_type == FlipBipFileDat) {
        path = FLIPBIP_DAT_PATH;
        path_bak = FLIPBIP_DAT_PATH_BAK;
    } else {
        char path_buf[FILE_MAX_PATH_LEN] = {0};
        strcpy(path_buf, FLIPBIP_APP_BASE_FOLDER); // 22
        strcpy(path_buf + strlen(path_buf), "/");
        strcpy(path_buf + strlen(path_buf), file_name);
        path = path_buf;
        path_bak = NULL;
    }
    int open_mode = FSOM_OPEN_ALWAYS;
    if(append) {
        open_mode = FSOM_OPEN_APPEND;
    }

    Storage* fs_api = furi_record_open(RECORD_STORAGE);
    // // if the key file exists, we don't want to overwrite it
    // if (key_file && storage_file_exists(fs_api, path)) {
    //     furi_record_close(RECORD_STORAGE);
    //     ret = true;
    //     return ret;
    // }
    // try to create the folder
    storage_simply_mkdir(fs_api, FLIPBIP_APP_BASE_FOLDER);

    File* settings_file = storage_file_alloc(fs_api);
    if(storage_file_open(settings_file, path, FSAM_WRITE, open_mode)) {
        storage_file_write(settings_file, settings, strlen(settings));
        storage_file_write(settings_file, "\n", 1);
        ret = true;
    }
    storage_file_close(settings_file);
    storage_file_free(settings_file);

    if(path_bak != NULL) {
        File* settings_file_bak = storage_file_alloc(fs_api);
        if(storage_file_open(settings_file_bak, path_bak, FSAM_WRITE, open_mode)) {
            storage_file_write(settings_file_bak, settings, strlen(settings));
            storage_file_write(settings_file_bak, "\n", 1);
        }
        storage_file_close(settings_file_bak);
        storage_file_free(settings_file_bak);
    }

    furi_record_close(RECORD_STORAGE);

    return ret;
}

bool flipbip_save_qrfile(
    const char* qr_msg_prefix,
    const char* qr_msg_content,
    const char* file_name) {
    char qr_buf[FILE_MAX_QRFILE_CONTENT] = {0};
    strcpy(qr_buf, TEXT_QRFILE);
    strcpy(qr_buf + strlen(qr_buf), qr_msg_prefix);
    strcpy(qr_buf + strlen(qr_buf), qr_msg_content);
    return flipbip_save_file(qr_buf, FlipBipFileOther, file_name, false);
}

bool flipbip_load_file_secure(char* settings) {
    const size_t dlen = FILE_HLEN + FILE_SLEN + 1;

    // allocate memory for key/data
    char* data = malloc(dlen);
    memzero(data, dlen);

    // load k2 from file
    if(!flipbip_load_file(data, FlipBipFileKey, NULL)) return false;

    // check header
    if(data[0] != FILE_HSTR[0] || data[1] != FILE_HSTR[1] || data[2] != FILE_HSTR[2] ||
       data[3] != FILE_HSTR[3]) {
        memzero(data, dlen);
        free(data);
        return false;
    }
    // seek --> header
    data += FILE_HLEN;

    // prepare k1
    uint8_t k1[64];
    flipbip_xtob(FILE_K1, k1, strlen(FILE_K1) / 2);

    // load k2 from file buffer (secured by k1)
    flipbip_cipher(k1, strlen(FILE_K1) / 2, data, data, FILE_KLEN);
    uint8_t k2[128];
    flipbip_xtob(data, k2, FILE_KLEN / 2);
    // zero k2 buffer
    memzero(data, FILE_KLEN);
    // seek <-- header
    data -= FILE_HLEN;

    // load data from file
    if(!flipbip_load_file(data, FlipBipFileDat, NULL)) return false;

    // check header
    if(data[0] != FILE_HSTR[0] || data[1] != FILE_HSTR[1] || data[2] != FILE_HSTR[2] ||
       data[3] != FILE_HSTR[3]) {
        memzero(data, dlen);
        free(data);
        memzero(k1, strlen(FILE_K1) / 2);
        memzero(k2, FILE_KLEN / 2);
        return false;
    }
    // seek --> header
    data += FILE_HLEN;

    // load settings from file buffer (secured by k2)
    flipbip_cipher(k2, FILE_KLEN / 2, data, data, FILE_SLEN);
    flipbip_xtob(data, (unsigned char*)data, FILE_SLEN / 2);

    // copy to output
    strcpy(settings, data);

    // seek <-- header
    data -= FILE_HLEN;

    // clear memory
    memzero(data, dlen);
    free(data);
    memzero(k1, strlen(FILE_K1) / 2);
    memzero(k2, FILE_KLEN / 2);

    return true;
}

bool flipbip_save_file_secure(const char* settings) {
    const size_t dlen = FILE_HLEN + FILE_SLEN + 1;

    // cap settings to 256 bytes
    size_t len = strlen(settings);
    if(len > (FILE_SLEN / 2)) len = FILE_SLEN / 2;

    // allocate memory for key/data
    char* data = malloc(dlen);
    memzero(data, dlen);

    // write header
    strncpy(data, FILE_HSTR, FILE_HLEN);
    // seek --> header
    data += FILE_HLEN;

    // prepare k1
    uint8_t k1[64];
    flipbip_xtob(FILE_K1, k1, strlen(FILE_K1) / 2);

    // generate k2
    uint8_t k2[128];
    random_buffer(k2, FILE_KLEN / 2);

    // write k2 to file buffer (secured by k1)
    flipbip_btox(k2, FILE_KLEN / 2, data);
    flipbip_cipher(k1, strlen(FILE_K1) / 2, data, data, FILE_KLEN);

    // seek <-- header
    data -= FILE_HLEN;
    // save k2 to file
    flipbip_save_file(data, FlipBipFileKey, NULL, false);
    // seek --> header
    data += FILE_HLEN;
    // zero k2 memory
    memzero(data, FILE_KLEN);

    // write settings to file buffer (secured by k2)
    flipbip_btox((uint8_t*)settings, len, data);
    flipbip_cipher(k2, FILE_KLEN / 2, data, data, FILE_SLEN);

    // seek <-- header
    data -= FILE_HLEN;
    // save data to file
    flipbip_save_file(data, FlipBipFileDat, NULL, false);

    // clear memory
    memzero(data, dlen);
    free(data);
    memzero(k1, strlen(FILE_K1) / 2);
    memzero(k2, FILE_KLEN / 2);

    return true;
}
