#include "flipchess_file.h"
#include <storage/storage.h>
#include <loader/loader.h>

// #define FLIPCHESS_APP_BASE_FOLDER APP_BOARDA_PATH("flipchess")
#define FLIPCHESS_APP_BASE_FOLDER EXT_PATH("apps_data/flipchess")
#define FLIPCHESS_APP_BASE_FOLDER_PATH(path) FLIPCHESS_APP_BASE_FOLDER "/" path
#define FLIPCHESS_BOARD_FILE_NAME "board_fen.txt"
#define FLIPCHESS_BOARD_FILE_NAME_BAK "board_fen.bak"
#define FLIPCHESS_BOARD_PATH FLIPCHESS_APP_BASE_FOLDER_PATH(FLIPCHESS_BOARD_FILE_NAME)
#define FLIPCHESS_BOARD_PATH_BAK FLIPCHESS_APP_BASE_FOLDER_PATH(FLIPCHESS_BOARD_FILE_NAME_BAK)

#define FILE_MAX_PATH_LEN 48
#define FILE_MAX_CHARS 94

bool flipchess_has_file(const FlipChessFile file_type, const char* file_name, const bool remove) {
    bool ret = false;
    const char* path;
    if(file_type == FlipChessFileBoard) {
        path = FLIPCHESS_BOARD_PATH;
    } else {
        char path_buf[FILE_MAX_PATH_LEN] = {0};
        strcpy(path_buf, FLIPCHESS_APP_BASE_FOLDER); // 22
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

bool flipchess_load_file(char* contents, const FlipChessFile file_type, const char* file_name) {
    bool ret = false;
    const char* path;
    if(file_type == FlipChessFileBoard) {
        path = FLIPCHESS_BOARD_PATH;
    } else {
        char path_buf[FILE_MAX_PATH_LEN] = {0};
        strcpy(path_buf, FLIPCHESS_APP_BASE_FOLDER); // 22
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
              !storage_file_eof(settings_file)) {
            if(i < FILE_MAX_CHARS) {
                contents[i] = chr;
            }
            i++;
        }
        ret = true;
    } else {
        contents[0] = '\0';
        ret = false;
    }
    storage_file_close(settings_file);
    storage_file_free(settings_file);
    furi_record_close(RECORD_STORAGE);

    if(strlen(contents) > 0) {
        Storage* fs_api = furi_record_open(RECORD_STORAGE);
        FileInfo layout_file_info;
        FS_Error file_check_err = storage_common_stat(fs_api, path, &layout_file_info);
        furi_record_close(RECORD_STORAGE);
        if(file_check_err != FSE_OK) {
            contents[0] = '\0';
            ret = false;
        }
        // if(layout_file_info.size != 256) {
        //     memzero(settings, strlen(settings));
        //     settings[0] = '\0';
        // }
    }

    return ret;
}

bool flipchess_save_file(
    const char* settings,
    const FlipChessFile file_type,
    const char* file_name,
    const bool append,
    const bool overwrite) {
    bool ret = false;
    const char* path;
    const char* path_bak;
    if(file_type == FlipChessFileBoard) {
        path = FLIPCHESS_BOARD_PATH;
        path_bak = FLIPCHESS_BOARD_PATH_BAK;
    } else {
        char path_buf[FILE_MAX_PATH_LEN] = {0};
        strcpy(path_buf, FLIPCHESS_APP_BASE_FOLDER); // 22
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

    // try to create the folder
    storage_simply_mkdir(fs_api, FLIPCHESS_APP_BASE_FOLDER);

    if(overwrite) {
        storage_simply_remove(fs_api, path);
    }

    File* settings_file = storage_file_alloc(fs_api);
    if(storage_file_open(settings_file, path, FSAM_WRITE, open_mode)) {
        storage_file_write(settings_file, settings, strlen(settings));
        storage_file_write(settings_file, "\n", 1);
        ret = true;
    }
    storage_file_close(settings_file);
    storage_file_free(settings_file);

    if(path_bak != NULL) {
        if(overwrite) {
            storage_simply_remove(fs_api, path_bak);
        }

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
