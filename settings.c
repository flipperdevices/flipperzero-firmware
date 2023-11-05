#include "settings.h"

#define TAG "Gravity"

bool save_settings(UART_TerminalApp *app) {
    Storage *storage = furi_record_open(RECORD_STORAGE);
    File *file = storage_file_alloc(storage);

    if (!storage_file_open(file, APP_DATA_PATH("settings.dat"), FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        FURI_LOG_E(TAG, "Failed to open file");
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return false;
    }
    if (!storage_file_write(file, "foo", strlen("foo"))) {
        FURI_LOG_E(TAG, "Failed to write to file");
    }

    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);

    return true;
}

bool load_settings(UART_TerminalApp *app) {
    uint16_t bufferSize = 1024;
    char buffer[bufferSize];
    uint16_t bytesRead;

    Storage *storage = furi_record_open(RECORD_STORAGE);
    File *file = storage_file_alloc(storage);

    if (!storage_file_open(file, APP_DATA_PATH("settings.dat"), FSAM_READ, FSOM_OPEN_EXISTING)) {
        FURI_LOG_I(TAG, "Failed to open file. Have you saved settings to Flipper?");
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return false;
    }

    bytesRead = storage_file_read(file, buffer, bufferSize);
    // Parse file

    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);

    return true;
}