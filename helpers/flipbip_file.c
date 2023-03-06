#include "flipbip_file.h"
#include "../flipbip.h"

#include "../crypto/memzero.h"

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
        memzero(settings, strlen(settings));
        settings[0] = '\0';
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

bool flipbip_save_settings(const char* settings) {
    Storage* fs_api = furi_record_open(RECORD_STORAGE);
    File* settings_file = storage_file_alloc(fs_api);
    storage_common_mkdir(fs_api, FLIPBIP_APP_BASE_FOLDER);
    if(storage_file_open(settings_file, FLIPBIP_SETTINGS_PATH, FSAM_WRITE, FSOM_OPEN_ALWAYS)) {
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