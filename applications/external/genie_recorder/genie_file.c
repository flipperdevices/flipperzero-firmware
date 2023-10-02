#include <storage/storage.h>
#include <flipper_format.h>

#define GENIE_APPS_DATA_FOLDER EXT_PATH("apps_data")
#define GENIE_SAVE_FOLDER      \
    GENIE_APPS_DATA_FOLDER "/" \
                           "genie"
#define GENIE_SAVE_NAME "keys"
#define GENIE_SAVE_EXTENSION ".txt"

#ifdef TAG
#undef TAG
#endif
#define TAG "GenieFile"

static void ensure_dir_exists(Storage* storage, char* dir) {
    if(!storage_dir_exists(storage, dir)) {
        FURI_LOG_I(TAG, "Creating directory: %s", dir);
        storage_simply_mkdir(storage, dir);
    } else {
        FURI_LOG_D(TAG, "Directory exists: %s", dir);
    }
}

static void ensure_save_folder_exists(Storage* storage) {
    ensure_dir_exists(storage, GENIE_APPS_DATA_FOLDER);
    ensure_dir_exists(storage, GENIE_SAVE_FOLDER);
}

Storage* _storage;
File* _file;

void genie_file_init() {
    _storage = furi_record_open(RECORD_STORAGE);
    ensure_save_folder_exists(_storage);
    _file = storage_file_alloc(_storage);
    storage_file_open(
        _file,
        (GENIE_SAVE_FOLDER "/" GENIE_SAVE_NAME GENIE_SAVE_EXTENSION),
        FSAM_WRITE,
        FSOM_OPEN_APPEND);
}

void genie_file_close() {
    if(_file) {
        storage_file_close(_file);
        _file = NULL;
    }

    if(_storage) {
        furi_record_close(RECORD_STORAGE);
        _storage = NULL;
    }
}

void demo() {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);
    if(storage_file_open(
           file,
           (GENIE_SAVE_FOLDER "/" GENIE_SAVE_NAME GENIE_SAVE_EXTENSION),
           FSAM_WRITE,
           FSOM_OPEN_APPEND)) {
        if(!storage_file_write(_file, "Hello\r\n", 7)) {
            FURI_LOG_E(TAG, "Failed to write to file");
        }
    } else {
        FURI_LOG_E(TAG, "Failed to open file");
    }
    storage_file_close(file);
    furi_record_close(RECORD_STORAGE);
}

bool genie_save(uint32_t count, FuriString* key) {
    bool success = false;
    //    Storage* storage = furi_record_open(RECORD_STORAGE);

    char buffer[8 + 1 + 16 + 2 + 1] = {0};
    snprintf(buffer, 8 + 1 + 16 + 2 + 1, "%08lX,%s\r\n", count, furi_string_get_cstr(key));

    //    File* file = NULL;
    do {
        //        if(!storage) {
        //            FURI_LOG_E(TAG, "Failed to access storage");
        //            break;
        //        }

        //        file = storage_file_alloc(storage);
        //        if(storage_file_open(
        //               file,
        //               (GENIE_SAVE_FOLDER "/" GENIE_SAVE_NAME GENIE_SAVE_EXTENSION),
        //               FSAM_WRITE,
        //               FSOM_OPEN_APPEND))
        if(_file == NULL) {
            genie_file_init();
        }

        if(_file) {
            if(!storage_file_write(_file, buffer, COUNT_OF(buffer) - 1)) {
                FURI_LOG_E(TAG, "Failed to write to file");
                break;
            }

            storage_file_sync(_file);

            success = true;
        }
    } while(false);

    //    if(file) {
    //        storage_file_close(file);
    //    }

    //    furi_record_close(RECORD_STORAGE);
    return success;
}

uint32_t genie_load() {
    uint32_t count = 0;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FuriString* file_path = furi_string_alloc();

    FuriString* buffer = furi_string_alloc();

    furi_string_printf(
        file_path, "%s/%s%s", GENIE_SAVE_FOLDER, GENIE_SAVE_NAME, GENIE_SAVE_EXTENSION);

    File* file = NULL;
    do {
        if(!storage) {
            FURI_LOG_E(TAG, "Failed to access storage");
            break;
        }

        ensure_save_folder_exists(storage);

        file = storage_file_alloc(storage);
        if(storage_file_open(
               file, furi_string_get_cstr(file_path), FSAM_READ, FSOM_OPEN_EXISTING)) {
            FURI_LOG_D(TAG, "Reading file: %s", furi_string_get_cstr(file_path));

            char data[8 + 1 + 16 + 2 + 1] = {0};

            while(storage_file_read(file, data, 8 + 1 + 16 + 2)) {
                sscanf(data, "%08lX", &count);
                FURI_LOG_D(TAG, "Read: %s, count: %ld", data, count);
            }
        }
    } while(false);

    if(file) {
        storage_file_close(file);
    }

    furi_string_free(file_path);
    furi_string_free(buffer);
    furi_record_close(RECORD_STORAGE);
    return count;
}