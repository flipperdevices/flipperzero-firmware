#include "meal_pager_storage.h"

static Storage* meal_pager_open_storage() {
    return furi_record_open(RECORD_STORAGE);
}

static void meal_pager_close_storage() {
    furi_record_close(RECORD_STORAGE);
}

static void meal_pager_close_config_file(FlipperFormat* file) {
    if(file == NULL) return;
    flipper_format_rewind(file);
    flipper_format_file_close(file);
    flipper_format_free(file);
}

bool meal_pager_save_subghz_buffer_file_start(
    void* context,
    FlipperFormat* ff,
    Storage* storage,
    char* frequency) {
    // SubGhz TXRX can only be loaded with files, makes sense as to save RAM
    Meal_Pager* app = context;
    UNUSED(app);
    bool success = false;
    FURI_LOG_D(TAG, "Creating Temp File");
    //Storage* storage = furi_record_open(RECORD_STORAGE);
    //FlipperFormat* ff = flipper_format_file_alloc(storage);

    // Overwrite wont work, so delete first
    if(storage_file_exists(storage, MEAL_PAGER_TMP_FILE)) {
        bool stored = storage_simply_remove(storage, MEAL_PAGER_TMP_FILE);
        if(!stored) {
            FURI_LOG_D(TAG, "Cannot remove file, seems to be open");
            return success;
        }
    }

    // Open File, create if not exists
    if(!storage_common_stat(storage, MEAL_PAGER_TMP_FILE, NULL) == FSE_OK) {
        FURI_LOG_D(
            TAG, "Config file %s is not found. Will create new.", MEAL_PAGER_SETTINGS_SAVE_PATH);
        if(storage_common_stat(storage, CONFIG_FILE_DIRECTORY_PATH, NULL) == FSE_NOT_EXIST) {
            FURI_LOG_D(
                TAG, "Directory %s doesn't exist. Will create new.", CONFIG_FILE_DIRECTORY_PATH);
            if(!storage_simply_mkdir(storage, CONFIG_FILE_DIRECTORY_PATH)) {
                FURI_LOG_D(TAG, "Error creating directory %s", CONFIG_FILE_DIRECTORY_PATH);
            }
        }
    }

    if(!flipper_format_file_open_new(ff, MEAL_PAGER_TMP_FILE)) {
        //totp_close_config_file(fff_file);
        FURI_LOG_D(TAG, "Error creating new file %s", MEAL_PAGER_TMP_FILE);
        meal_pager_close_storage();
        return success;
    }

    success = flipper_format_write_header_cstr(
                  ff, MEAL_PAGER_SUBGHZ_FILE_TYPE, MEAL_PAGER_SUBGHZ_FILE_VERSION) &&
              flipper_format_write_string_cstr(ff, "Frequency", frequency) &&
              flipper_format_write_string_cstr(ff, "Preset", MEAL_PAGER_SUBGHZ_FILE_PRESET) &&
              flipper_format_write_string_cstr(ff, "Protocol", MEAL_PAGER_SUBGHZ_FILE_Protocol);
    return success;
}

void meal_pager_save_subghz_buffer_stop(void* context, FlipperFormat* ff) {
    Meal_Pager* app = context;
    UNUSED(app);
    FURI_LOG_D(TAG, "Closing Temp File");
    if(!flipper_format_rewind(ff)) {
        meal_pager_close_config_file(ff);
        FURI_LOG_E(TAG, "Rewind error");
        meal_pager_close_storage();
        return;
    }

    meal_pager_close_config_file(ff);
    meal_pager_close_storage();
}

void meal_pager_save_settings(void* context) {
    Meal_Pager* app = context;
    if(app->save_settings == 0) {
        FURI_LOG_D(TAG, "Skipping Save because Disabled");
        return;
    }

    FURI_LOG_D(TAG, "Saving Settings to File");
    Storage* storage = meal_pager_open_storage();
    FlipperFormat* fff_file = flipper_format_file_alloc(storage);

    // Overwrite wont work, so delete first
    if(storage_file_exists(storage, MEAL_PAGER_SETTINGS_SAVE_PATH)) {
        storage_simply_remove(storage, MEAL_PAGER_SETTINGS_SAVE_PATH);
    }

    // Open File, create if not exists
    if(!storage_common_stat(storage, MEAL_PAGER_SETTINGS_SAVE_PATH, NULL) == FSE_OK) {
        FURI_LOG_D(
            TAG, "Config file %s is not found. Will create new.", MEAL_PAGER_SETTINGS_SAVE_PATH);
        if(storage_common_stat(storage, CONFIG_FILE_DIRECTORY_PATH, NULL) == FSE_NOT_EXIST) {
            FURI_LOG_D(
                TAG, "Directory %s doesn't exist. Will create new.", CONFIG_FILE_DIRECTORY_PATH);
            if(!storage_simply_mkdir(storage, CONFIG_FILE_DIRECTORY_PATH)) {
                FURI_LOG_E(TAG, "Error creating directory %s", CONFIG_FILE_DIRECTORY_PATH);
            }
        }
    }

    if(!flipper_format_file_open_new(fff_file, MEAL_PAGER_SETTINGS_SAVE_PATH)) {
        //totp_close_config_file(fff_file);
        FURI_LOG_E(TAG, "Error creating new file %s", MEAL_PAGER_SETTINGS_SAVE_PATH);
        meal_pager_close_storage();
        return;
    }

    // Store Settings
    flipper_format_write_header_cstr(
        fff_file, MEAL_PAGER_SETTINGS_HEADER, MEAL_PAGER_SETTINGS_FILE_VERSION);
    flipper_format_write_uint32(fff_file, MEAL_PAGER_SETTINGS_KEY_PAGER_TYPE, &app->pager_type, 1);
    flipper_format_write_uint32(
        fff_file, MEAL_PAGER_SETTINGS_KEY_FIRST_STATION, &app->first_station, 1);
    flipper_format_write_uint32(
        fff_file, MEAL_PAGER_SETTINGS_KEY_LAST_STATION, &app->last_station, 1);
    flipper_format_write_uint32(
        fff_file, MEAL_PAGER_SETTINGS_KEY_FIRST_PAGER, &app->first_pager, 1);
    flipper_format_write_uint32(fff_file, MEAL_PAGER_SETTINGS_KEY_LAST_PAGER, &app->last_pager, 1);
    flipper_format_write_uint32(fff_file, MEAL_PAGER_SETTINGS_KEY_LAST_PAGER, &app->last_pager, 1);
    flipper_format_write_uint32(fff_file, MEAL_PAGER_SETTINGS_KEY_REPEATS, &app->repeats, 1);
    flipper_format_write_uint32(fff_file, MEAL_PAGER_SETTINGS_KEY_HAPTIC, &app->haptic, 1);
    flipper_format_write_uint32(fff_file, MEAL_PAGER_SETTINGS_KEY_SPEAKER, &app->speaker, 1);
    flipper_format_write_uint32(fff_file, MEAL_PAGER_SETTINGS_KEY_LED, &app->led, 1);
    flipper_format_write_uint32(
        fff_file, MEAL_PAGER_SETTINGS_KEY_SAVE_SETTINGS, &app->save_settings, 1);

    if(!flipper_format_rewind(fff_file)) {
        meal_pager_close_config_file(fff_file);
        FURI_LOG_E(TAG, "Rewind error");
        meal_pager_close_storage();
        return;
    }

    meal_pager_close_config_file(fff_file);
    meal_pager_close_storage();
}

void meal_pager_read_settings(void* context) {
    Meal_Pager* app = context;
    Storage* storage = meal_pager_open_storage();
    FlipperFormat* fff_file = flipper_format_file_alloc(storage);

    if(storage_common_stat(storage, MEAL_PAGER_SETTINGS_SAVE_PATH, NULL) != FSE_OK) {
        meal_pager_close_config_file(fff_file);
        meal_pager_close_storage();
        return;
    }
    uint32_t file_version;
    FuriString* temp_str = furi_string_alloc();

    if(!flipper_format_file_open_existing(fff_file, MEAL_PAGER_SETTINGS_SAVE_PATH)) {
        FURI_LOG_E(TAG, "Cannot open file %s", MEAL_PAGER_SETTINGS_SAVE_PATH);
        meal_pager_close_config_file(fff_file);
        meal_pager_close_storage();
        furi_string_free(temp_str);
        return;
    }

    if(!flipper_format_read_header(fff_file, temp_str, &file_version)) {
        FURI_LOG_E(TAG, "Missing Header Data");
        meal_pager_close_config_file(fff_file);
        meal_pager_close_storage();
        furi_string_free(temp_str);
        return;
    }

    if(file_version < MEAL_PAGER_SETTINGS_FILE_VERSION) {
        FURI_LOG_I(TAG, "old config version, will be removed.");
        meal_pager_close_config_file(fff_file);
        meal_pager_close_storage();
        furi_string_free(temp_str);
        return;
    }

    flipper_format_read_uint32(fff_file, MEAL_PAGER_SETTINGS_KEY_PAGER_TYPE, &app->pager_type, 1);
    meal_pager_set_max_values(app);
    flipper_format_read_uint32(
        fff_file, MEAL_PAGER_SETTINGS_KEY_FIRST_STATION, &app->first_station, 1);
    flipper_format_read_uint32(
        fff_file, MEAL_PAGER_SETTINGS_KEY_LAST_STATION, &app->last_station, 1);
    flipper_format_read_uint32(
        fff_file, MEAL_PAGER_SETTINGS_KEY_FIRST_PAGER, &app->first_pager, 1);
    flipper_format_read_uint32(fff_file, MEAL_PAGER_SETTINGS_KEY_LAST_PAGER, &app->last_pager, 1);
    flipper_format_read_uint32(fff_file, MEAL_PAGER_SETTINGS_KEY_REPEATS, &app->repeats, 1);
    flipper_format_read_uint32(fff_file, MEAL_PAGER_SETTINGS_KEY_HAPTIC, &app->haptic, 1);
    flipper_format_read_uint32(fff_file, MEAL_PAGER_SETTINGS_KEY_SPEAKER, &app->speaker, 1);
    flipper_format_read_uint32(fff_file, MEAL_PAGER_SETTINGS_KEY_LED, &app->led, 1);
    flipper_format_read_uint32(
        fff_file, MEAL_PAGER_SETTINGS_KEY_SAVE_SETTINGS, &app->save_settings, 1);

    flipper_format_rewind(fff_file);

    furi_string_free(temp_str);

    meal_pager_close_config_file(fff_file);
    meal_pager_close_storage();
}

void meal_pager_set_max_values(void* context) {
    Meal_Pager* app = context;
    switch(app->pager_type) {
    case Meal_PagerPagerTypeT119:
    case Meal_PagerPagerTypeTD165:
        app->max_station = 8191;
        app->max_pager = 999;
        break;
    case Meal_PagerPagerTypeTD174:
        app->max_station = 8191;
        app->max_pager = 10;
        break;
    case Meal_PagerPagerTypeTD157:
        app->max_station = 1023;
        app->max_pager = 999;
        break;
    }
    if(app->first_station > app->max_station) {
        app->first_station = app->max_station;
        snprintf(app->text_store[0], sizeof(app->text_store[0]), "%lu", app->first_station);
    }
    if(app->last_station > app->max_station) {
        app->last_station = app->max_station;
        snprintf(app->text_store[1], sizeof(app->text_store[1]), "%lu", app->last_station);
    }
    if(app->last_station < app->first_station) {
        app->last_station = app->first_station;
        snprintf(app->text_store[1], sizeof(app->text_store[1]), "%lu", app->last_station);
    }
    if(app->first_pager > app->max_pager) {
        app->first_pager = app->max_pager;
        snprintf(app->text_store[2], sizeof(app->text_store[2]), "%lu", app->first_pager);
    }
    if(app->last_pager > app->max_pager) {
        app->last_pager = app->max_pager;
        snprintf(app->text_store[3], sizeof(app->text_store[3]), "%lu", app->last_pager);
    }
    if(app->last_pager < app->first_pager) {
        app->last_pager = app->first_pager;
        snprintf(app->text_store[3], sizeof(app->text_store[3]), "%lu", app->last_pager);
    }
}