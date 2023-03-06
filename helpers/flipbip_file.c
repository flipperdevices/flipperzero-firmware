#include "flipbip_file.h"
#include "../helpers/flipbip_string.h"

#include "../crypto/memzero.h"
#include "../crypto/rand.h"

#include <storage/storage.h>

#define FLIPBIP_APP_BASE_FOLDER EXT_PATH("apps_data/flipbip")
// #define FLIPBIP_SETTINGS_FILE_NAME ".flipbip.dat"
#define FLIPBIP_SETTINGS_FILE_NAME ".flipbip.txt"
#define FLIPBIP_SETTINGS_PATH FLIPBIP_APP_BASE_FOLDER "/" FLIPBIP_SETTINGS_FILE_NAME

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
        strcpy(settings, "uhoh");
        //memzero(settings, strlen(settings));
        //settings[0] = '\0';
    }
    storage_file_close(settings_file);
    storage_file_free(settings_file);
    furi_record_close(RECORD_STORAGE);

    // if(!strlen(settings) == 0) {
    //     Storage* fs_api = furi_record_open(RECORD_STORAGE);
    //     FileInfo layout_file_info;
    //     FS_Error file_check_err = storage_common_stat(
    //         fs_api, FLIPBIP_SETTINGS_PATH, &layout_file_info);
    //     furi_record_close(RECORD_STORAGE);
    //     if(file_check_err != FSE_OK) {
    //         memzero(settings, strlen(settings));
    //         settings[0] = '\0';
    //         return;
    //     }
    //     if(layout_file_info.size != 256) {
    //         memzero(settings, strlen(settings));
    //         settings[0] = '\0';
    //     }
    // }

    return true;
}

bool flipbip_save_settings(const char* settings, bool append) {
    Storage* fs_api = furi_record_open(RECORD_STORAGE);
    File* settings_file = storage_file_alloc(fs_api);
    storage_common_mkdir(fs_api, FLIPBIP_APP_BASE_FOLDER);
    int open_mode = FSOM_OPEN_ALWAYS;
    if(append) {
        open_mode = FSOM_OPEN_APPEND;
    }
    if(storage_file_open(settings_file, FLIPBIP_SETTINGS_PATH, FSAM_WRITE, open_mode)) {
        storage_file_write(
            settings_file,
            settings,
            strlen(settings));
        storage_file_write(settings_file, "\n", 1);
    }
    storage_file_close(settings_file);
    storage_file_free(settings_file);
    furi_record_close(RECORD_STORAGE);

    return true;
}

bool flipbip_load_settings_secure(char* settings) {
    const size_t hlen = 4;
    const size_t klen = 128;
    const size_t slen = 512;
    const size_t dlen = hlen + klen + slen;
    
    char *data = malloc(dlen+1);
    memzero(data, dlen+1);

    if (!flipbip_load_settings(data)) return false;

    // if (strncmp(data, "fb01", hlen) != 0) {
    //     memzero(data, dlen);
    //     free(data);
    //     return true;
    // }
    data += hlen;

    uint8_t key[64];
    flipbip_xtob(data, key, 64);
    data += klen;

    flipbip_cipher(key, data, data);
    flipbip_xtob(data, (unsigned char*)settings, 256);

    data = data - klen - hlen;
    memzero(data, dlen);
    free(data);

    return true;
}

bool flipbip_save_settings_secure(const char* settings) {
    const size_t hlen = 4;
    const size_t klen = 128;
    const size_t slen = 512;
    const size_t dlen = hlen + klen + slen;

    size_t len = strlen(settings);
    if (len > 256) len = 256;
    
    char *data = malloc(dlen + 1);
    memzero(data, dlen + 1);
    
    memcpy(data, "fb01", hlen);
    data += hlen - 1;

    uint8_t key[64];
    random_buffer(key, 64);
    for (size_t i = 0; i < 64; i++) {
        flipbip_btox(key[i], data + (i * 2));
    }
    data += klen;

    for (size_t i = 0; i < len; i++) {
        flipbip_btox(settings[i], data + (i * 2));
    }
    flipbip_cipher(key, data, data);

    data = data - klen - hlen;
    data[dlen] = '\0';

    flipbip_save_settings(data, false);

    memzero(data, dlen);
    free(data);

    return true;
}