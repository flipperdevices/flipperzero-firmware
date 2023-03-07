#include "flipbip_file.h"
#include "../helpers/flipbip_string.h"

#include "../crypto/memzero.h"
#include "../crypto/rand.h"

#include <storage/storage.h>

#define FLIPBIP_APP_BASE_FOLDER EXT_PATH("apps_data/flipbip")
// #define FLIPBIP_SETTINGS_FILE_NAME ".flipbip.dat"
#define FLIPBIP_SETTINGS_FILE_NAME ".flipbip.txt"
#define FLIPBIP_SETTINGS_FILE_NAME_BAK ".flipbip.bak"
#define FLIPBIP_SETTINGS_PATH FLIPBIP_APP_BASE_FOLDER "/" FLIPBIP_SETTINGS_FILE_NAME
#define FLIPBIP_SETTINGS_PATH_BAK FLIPBIP_APP_BASE_FOLDER "/" FLIPBIP_SETTINGS_FILE_NAME_BAK

const size_t FILE_HLEN = 4;
const size_t FILE_KLEN = 128;
const size_t FILE_SLEN = 512;
const char* FILE_HSTR = "fb01";
const char* FILE_K1 = "fb0131d5cf688221c109163908ebe51debb46227c6cc8b37641910833222772a"
                      "baefe6d9ceb651842260e0d1e05e3b90d15e7d5ffaaabc0207bf200a117793a2";

bool flipbip_load_settings(char* settings) {
    Storage *fs_api = furi_record_open(RECORD_STORAGE);
    File* settings_file = storage_file_alloc(fs_api);
    if(storage_file_open(settings_file, FLIPBIP_SETTINGS_PATH, FSAM_READ, FSOM_OPEN_EXISTING)) {
        char chr;
        int i = 0;
        while((storage_file_read(settings_file, &chr, 1) == 1) &&
              !storage_file_eof(settings_file) && !isspace(chr)) {
            settings[i] = chr;
            i++;
        }
    } else {
        memzero(settings, strlen(settings));
    }
    storage_file_close(settings_file);
    storage_file_free(settings_file);
    furi_record_close(RECORD_STORAGE);

    if(!strlen(settings) == 0) {
        Storage* fs_api = furi_record_open(RECORD_STORAGE);
        FileInfo layout_file_info;
        FS_Error file_check_err = storage_common_stat(
            fs_api, FLIPBIP_SETTINGS_PATH, &layout_file_info);
        furi_record_close(RECORD_STORAGE);
        if(file_check_err != FSE_OK) {
            memzero(settings, strlen(settings));
            settings[0] = '\0';
            return false;
        }
        // if(layout_file_info.size != 256) {
        //     memzero(settings, strlen(settings));
        //     settings[0] = '\0';
        // }
    }

    return true;
}

bool flipbip_save_settings(const char* settings, bool append) {
    Storage* fs_api = furi_record_open(RECORD_STORAGE);
    
    storage_common_mkdir(fs_api, FLIPBIP_APP_BASE_FOLDER);
    int open_mode = FSOM_OPEN_ALWAYS;
    if(append) {
        open_mode = FSOM_OPEN_APPEND;
    }

    File* settings_file = storage_file_alloc(fs_api);
    if(storage_file_open(settings_file, FLIPBIP_SETTINGS_PATH, FSAM_WRITE, open_mode)) {
        storage_file_write(
            settings_file,
            settings,
            strlen(settings));
        storage_file_write(settings_file, "\n", 1);
    }
    storage_file_close(settings_file);
    storage_file_free(settings_file);

    File* settings_file_bak = storage_file_alloc(fs_api);
    if(storage_file_open(settings_file_bak, FLIPBIP_SETTINGS_PATH_BAK, FSAM_WRITE, open_mode)) {
        storage_file_write(
            settings_file_bak,
            settings,
            strlen(settings));
        storage_file_write(settings_file_bak, "\n", 1);
    }
    storage_file_close(settings_file_bak);
    storage_file_free(settings_file_bak);

    furi_record_close(RECORD_STORAGE);

    return true;
}

bool flipbip_load_settings_secure(char* settings) {
    const size_t dlen = FILE_HLEN + FILE_KLEN + FILE_SLEN + 1;
    
    // allocate memory for data
    char *data = malloc(dlen);
    memzero(data, dlen);

    // load data from file
    if (!flipbip_load_settings(data)) return false;

    // check header
    if (data[0] != FILE_HSTR[0] || data[1] != FILE_HSTR[1] || data[2] != FILE_HSTR[2] || data[3] != FILE_HSTR[3]) {
        memzero(data, dlen);
        free(data);
        return false;
    }
    data += FILE_HLEN;

    // load k2 from file using k1
    //uint8_t k1[64];
    //flipbip_xtob(FILE_K1, k1, 64);
    uint8_t k2[64];
    //flipbip_cipher(k1, data, data, FILE_KLEN);
    flipbip_xtob(data, k2, 64);
    data += FILE_KLEN;

    // load settings from file using k2
    flipbip_cipher(k2, data, data, FILE_SLEN);
    flipbip_xtob(data, (unsigned char*)settings, 256);

    data = data - FILE_KLEN - FILE_HLEN;
    
    // clear memory
    memzero(data, dlen);
    free(data);

    return true;
}

bool flipbip_save_settings_secure(const char* settings) {
    const size_t dlen = FILE_HLEN + FILE_KLEN + FILE_SLEN + 1;

    // cap settings to 256 bytes
    size_t len = strlen(settings);
    if (len > 256) len = 256;
    
    // allocate memory for data
    char *data = malloc(dlen + 1);
    memzero(data, dlen + 1);
    
    // write header
    strncpy(data, FILE_HSTR, FILE_HLEN);
    data += FILE_HLEN;

    // generate key
    //uint8_t k1[64];
    //flipbip_xtob(FILE_K1, k1, 64);
    uint8_t k2[64];
    random_buffer(k2, 64);

    // write k2 to file (secured by k1)
    for (size_t i = 0; i < 64; i++) {
        flipbip_btox(k2[i], data + (i * 2));
    }
    //flipbip_cipher(k1, data, data, FILE_KLEN);
    data += FILE_KLEN;

    // write settings to file (secured by k2)
    for (size_t i = 0; i < len; i++) {
        flipbip_btox(settings[i], data + (i * 2));
    }
    flipbip_cipher(k2, data, data, FILE_SLEN);

    data = data - FILE_KLEN - FILE_HLEN;

    // save data
    flipbip_save_settings(data, false);

    // clear memory
    memzero(data, dlen);
    free(data);

    return true;
}