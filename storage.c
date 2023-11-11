#include "storage.h"

#define TAG "Gravity"
#define FILENAME_SETTINGS "gravity.settings"
#define FILENAME_DATA "data.gravity"
#define DEFAULT_SSID_MIN 0
#define DEFAULT_SSID_MAX 32
#define DEFAULT_SSID_COUNT 20
#define DEFAULT_ATTACK_MILLIS 50
#define DEFAULT_PKT_EXPIRY 0 /* Don't expire */
#define FILEBUFFER_SIZE 128

/* Essentially the reverse of do_sync() from sync.c - Take all settings
   from Flipper-Gravity and write them to a file on the Flipper, using
   the same format as sync.
   YAGNI: We should be able to ignore the case of empty strings because
          these settings all have default values.
          Except for 'Get' menu options, which may still be "Get" if
          the sync on startup failed. Default values are #define'd
 */
bool writeSettingsToFile(UART_TerminalApp *app, File *file) {
    int bufLen = 0;
    char fBuffer[FILEBUFFER_SIZE] = "";
    char strBuffer[32] = "";

    memset(fBuffer, '\0', FILEBUFFER_SIZE);
    for (GravitySyncItem i = 0; i < GRAVITY_SYNC_ITEM_COUNT; ++i) {
        sprintf(strBuffer, "(%d:", i);
        strcpy(&(fBuffer[bufLen]), strBuffer);
        bufLen += strlen(strBuffer);
        switch (i) {
            case GRAVITY_SYNC_HOP_ON:
                sprintf(strBuffer, "%d)", app->selected_menu_options[GRAVITY_MENU_SETTINGS][SETTINGS_MENU_HOP_STATUS]);
                break;
            case GRAVITY_SYNC_SSID_MIN:
                /* If menu item's 'Get' option is "Get" then we have no value - Use default */
                if (strcmp(settings[SETTINGS_MENU_SSID_MIN].options_menu[OPTIONS_SSID_MIN_GET], STRINGS_GET)) {
                    sprintf(strBuffer, "%s)", settings[SETTINGS_MENU_SSID_MIN].options_menu[OPTIONS_SSID_MIN_GET]);
                } else {
                    sprintf(strBuffer, "%d)", DEFAULT_SSID_MIN);
                }
                break;
            case GRAVITY_SYNC_SSID_MAX:
                if (strcmp(settings[SETTINGS_MENU_SSID_MAX].options_menu[OPTIONS_SSID_MAX_GET], STRINGS_GET)) {
                    sprintf(strBuffer, "%s)", settings[SETTINGS_MENU_SSID_MAX].options_menu[OPTIONS_SSID_MAX_GET]);
                } else {
                    sprintf(strBuffer, "%d)", DEFAULT_SSID_MAX);
                }
                break;
            case GRAVITY_SYNC_SSID_COUNT:
                if (strcmp(settings[SETTINGS_MENU_SSID_DEFAULT].options_menu[OPTIONS_SSID_DEFAULT_GET], STRINGS_GET)) {
                    sprintf(strBuffer, "%s)", settings[SETTINGS_MENU_SSID_DEFAULT].options_menu[OPTIONS_SSID_DEFAULT_GET]);
                } else {
                    sprintf(strBuffer, "%d)", DEFAULT_SSID_COUNT);
                }
                break;
            case GRAVITY_SYNC_CHANNEL:
                /* Happily, the channel number is the same as the channel index */
                // TODO: If the "Get" option is removed this won't be true anymore and all of sync
                //       will need to be refactored
                sprintf(strBuffer, "%d)", app->channel);
                break;
            case GRAVITY_SYNC_MAC: ;
                char strTemp[NUM_MAC_BYTES * 3];
                if (!mac_bytes_to_string(app->mac_bytes, strTemp)) {
                    FURI_LOG_E(TAG, "Failed to stringify MAC to save settings, abandoning save");
                    return false;
                }
                sprintf(strBuffer, "%s)", strTemp);
                break;
            case GRAVITY_SYNC_ATTACK_MILLIS:
                if (strcmp(settings[SETTINGS_MENU_ATTACK_MILLIS].options_menu[OPTIONS_ATTACK_MILLIS_GET], STRINGS_GET)) {
                    sprintf(strBuffer, "%s)", settings[SETTINGS_MENU_ATTACK_MILLIS].options_menu[OPTIONS_ATTACK_MILLIS_GET]);
                } else {
                    sprintf(strBuffer, "%d)", DEFAULT_ATTACK_MILLIS);
                }
                break;
            case GRAVITY_SYNC_MAC_RAND:
                sprintf(strBuffer, "%d)", app->mac_rand);
                break;
            case GRAVITY_SYNC_PKT_EXPIRY:
                if (strcmp(settings[SETTINGS_MENU_PKT_EXPIRY].options_menu[OPTIONS_PKT_EXPIRY_GET], STRINGS_GET)) {
                    sprintf(strBuffer, "%s)", settings[SETTINGS_MENU_PKT_EXPIRY].options_menu[OPTIONS_PKT_EXPIRY_GET]);
                } else {
                    sprintf(strBuffer, "%d)", DEFAULT_PKT_EXPIRY);
                }
                break;
            case GRAVITY_SYNC_HOP_MODE:
                sprintf(strBuffer, "%d)", app->hopMode);
                break;
            case GRAVITY_SYNC_DICT_DISABLED:
                sprintf(strBuffer, "%d)", app->dict_disabled);
                break;
            case GRAVITY_SYNC_PURGE_STRAT:
                sprintf(strBuffer, "%d)", app->purgeStrategy);
                break;
            case GRAVITY_SYNC_PURGE_RSSI_MAX:
                sprintf(strBuffer, "%d)", app->purgeRSSI);
                break;
            case GRAVITY_SYNC_PURGE_AGE_MIN:
                sprintf(strBuffer, app->purgeAge);
                break;
            default:
                //
                break;
        }
        strcpy(&(fBuffer[bufLen]), strBuffer);
        bufLen += strlen(strBuffer);        
    }
    return storage_write_to_file(file, strBuffer, bufLen);
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
    /* Keeping this comment block as an example of writing arbitrary text to a file
    if (!storage_file_write(file, "foo", strlen("foo"))) {
        FURI_LOG_E(TAG, "Failed to write to file");
        close_file(file);
        return false;
    } */

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