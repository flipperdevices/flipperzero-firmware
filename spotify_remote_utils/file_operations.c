#include "file_operations.h"

void save_ssid(const char* ssid) {
    // We need a storage struct (gain access to the filesystem API )
    Storage* storage = furi_record_open(RECORD_STORAGE);

    // storage_file_alloc gives us a File pointer using the Storage API.
    File* file = storage_file_alloc(storage);

    bool file_open_ret = storage_file_open(
        file, EXT_PATH("apps/Examples/wifi.log"), FSAM_WRITE, FSOM_CREATE_ALWAYS);

    if(file_open_ret) {
        storage_file_write(file, ssid, strlen(ssid));
        storage_file_close(file);
        storage_file_free(file);
    }
}

void save_password(const char* password) {
    // We need a storage struct (gain access to the filesystem API )
    Storage* storage = furi_record_open(RECORD_STORAGE);

    // storage_file_alloc gives us a File pointer using the Storage API.
    File* file = storage_file_alloc(storage);

    bool file_open_ret =
        storage_file_open(file, EXT_PATH("apps/Examples/wifi.log"), FSAM_WRITE, FSOM_OPEN_APPEND);

    if(file_open_ret) {
        storage_file_write(file, password, strlen(password));
        storage_file_close(file);
        storage_file_free(file);
    }
}
