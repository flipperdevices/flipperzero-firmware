#include "storage.h"

#define TAG "Gravity"
#define FILENAME_SETTINGS "settings.dat"
#define FILENAME_DATA "gravity.dat"

/* Essentially the reverse of do_sync() from sync.c - Take all settings
   from Flipper-Gravity and write them to a file on the Flipper, using
   the same format as sync
 */
bool writeSettingsToFile(UART_TerminalApp *app, *file) {
    GravitySyncItem *syncItems = malloc(sizeof(GravitySyncItem) * GRAVITY_SYNC_ITEM_COUNT);
    if (syncItems == NULL) {
        FURI_LOG_E(TAG, "Failed allocating memory to hold settings metadata");
        return false;
    }
    for (int i = 0; i < GRAVITY_SYNC_ITEM_COUNT; ++i) {
        syncItems[i] = [i];
    }

    return true;
}

void close_file(File *file) {
    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
}

/* So much goddamn repeated code! The APP_DATA_PATH macro, as far as I can figure, can't be
   called with a variable, even a const char[] :( */
bool save_settings(UART_TerminalApp *app) {
    Storage *storage = furi_record_open(RECORD_STORAGE);
    File *file = storage_file_alloc(storage);
    if (!storage_file_open(file, APP_DATA_PATH(FILENAME_SETTINGS), FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        FURI_LOG_E(TAG, "Failed to open %s for writing", FILENAME_SETTINGS);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return false;
    }
    if (!storage_file_truncate(file)) {
        FURI_LOG_E(TAG, "Unable to truncate settings file for writing");
        close_file(file);
        return false;
    }
    if (!writeSettingsToFile(app, file)) {
        FURI_LOG_E(TAG, "Failed to write settings");
        close_file(file);
        return false;
    }
    if (!storage_file_write(file, "foo", strlen("foo"))) {
        FURI_LOG_E(TAG, "Failed to write to file");
        close_file(file);
        return false;
    }

    close_file(file);
    return true;
}

bool load_settings(UART_TerminalApp *app) {
    uint16_t bufferSize = 1024;
    char buffer[bufferSize];
    uint16_t bytesRead;

    Storage *storage = furi_record_open(RECORD_STORAGE);
    File *file = storage_file_alloc(storage);
    if (!storage_file_open(file, APP_DATA_PATH(FILENAME_SETTINGS), FSAM_READ, FSOM_OPEN_EXISTING)) {
        FURI_LOG_E(TAG, "Failed to open %s for reading", FILENAME_SETTINGS);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return false;
    }

    bytesRead = storage_file_read(file, buffer, bufferSize);
    // Parse file

    close_file(file);
    return true;
}

bool save_data(UART_TerminalApp *app) {
    Storage *storage = furi_record_open(RECORD_STORAGE);
    File *file = storage_file_alloc(storage);
    if (!storage_file_open(file, APP_DATA_PATH(FILENAME_DATA), FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        FURI_LOG_E(TAG, "Failed to open %s for writing", FILENAME_DATA);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return false;
    }
    if (!storage_file_truncate(file)) {
        FURI_LOG_E(TAG, "Unable to truncate data file for writing");
        close_file(file);
        return false;
    }
    if (!storage_file_write(file, "bar", strlen("bar"))) {
        FURI_LOG_E(TAG, "Failed to write data");
        close_file(file);
        return false;
    }

    close_file(file);
    return true;
}

bool load_data(UART_TerminalApp *app) {
    uint16_t bufferSize = 1024;
    char buffer[bufferSize];
    uint16_t bytesRead;

    Storage *storage = furi_record_open(RECORD_STORAGE);
    File *file = storage_file_alloc(storage);
    if (!storage_file_open(file, APP_DATA_PATH(FILENAME_DATA), FSAM_READ, FSOM_OPEN_EXISTING)) {
        FURI_LOG_E(TAG, "Failed to open %s - Have you saved data?", FILENAME_DATA);
        storage_file_close(file);
        furi_record_close(RECORD_STORAGE);
        return false;
    }

    bytesRead = storage_file_read(file, buffer, bufferSize);
    // Parse file

    close_file(file);
    return true;
}