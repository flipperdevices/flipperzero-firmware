#include "hex_viewer_storage.h"

static Storage* hex_viewer_open_storage() {
    return furi_record_open(RECORD_STORAGE);
}

static void hex_viewer_close_storage() {
    furi_record_close(RECORD_STORAGE);
}

static void hex_viewer_close_config_file(FlipperFormat* file) {
    if(file == NULL) return;
    flipper_format_file_close(file);
    flipper_format_free(file);
}

void hex_viewer_save_settings(void* context) {
    HexViewer* app = context;
    if(app->save_settings == 0) {
        return;
    }

    FURI_LOG_D(TAG, "Saving Settings");
    Storage* storage = hex_viewer_open_storage();
    FlipperFormat* fff_file = flipper_format_file_alloc(storage);

    // Overwrite wont work, so delete first
    if(storage_file_exists(storage, HEX_VIEWER_SETTINGS_SAVE_PATH)) {
        storage_simply_remove(storage, HEX_VIEWER_SETTINGS_SAVE_PATH);
    }

    // Open File, create if not exists
    if(!storage_common_stat(storage, HEX_VIEWER_SETTINGS_SAVE_PATH, NULL) == FSE_OK) {
        FURI_LOG_D(
            TAG, "Config file %s is not found. Will create new.", HEX_VIEWER_SETTINGS_SAVE_PATH);
        if(storage_common_stat(storage, CONFIG_FILE_DIRECTORY_PATH, NULL) == FSE_NOT_EXIST) {
            FURI_LOG_D(
                TAG, "Directory %s doesn't exist. Will create new.", CONFIG_FILE_DIRECTORY_PATH);
            if(!storage_simply_mkdir(storage, CONFIG_FILE_DIRECTORY_PATH)) {
                FURI_LOG_E(TAG, "Error creating directory %s", CONFIG_FILE_DIRECTORY_PATH);
            }
        }
    }

    if(!flipper_format_file_open_new(fff_file, HEX_VIEWER_SETTINGS_SAVE_PATH)) {
        //totp_close_config_file(fff_file);
        FURI_LOG_E(TAG, "Error creating new file %s", HEX_VIEWER_SETTINGS_SAVE_PATH);
        hex_viewer_close_storage();
        return;
    }

    // Store Settings
    flipper_format_write_header_cstr(
        fff_file, HEX_VIEWER_SETTINGS_HEADER, HEX_VIEWER_SETTINGS_FILE_VERSION);
    flipper_format_write_uint32(fff_file, HEX_VIEWER_SETTINGS_KEY_HAPTIC, &app->haptic, 1);
    flipper_format_write_uint32(fff_file, HEX_VIEWER_SETTINGS_KEY_SPEAKER, &app->speaker, 1);
    flipper_format_write_uint32(fff_file, HEX_VIEWER_SETTINGS_KEY_LED, &app->led, 1);
    flipper_format_write_uint32(
        fff_file, HEX_VIEWER_SETTINGS_KEY_SAVE_SETTINGS, &app->save_settings, 1);

    if(!flipper_format_rewind(fff_file)) {
        hex_viewer_close_config_file(fff_file);
        FURI_LOG_E(TAG, "Rewind error");
        hex_viewer_close_storage();
        return;
    }

    hex_viewer_close_config_file(fff_file);
    hex_viewer_close_storage();
}

void hex_viewer_read_settings(void* context) {
    HexViewer* app = context;
    Storage* storage = hex_viewer_open_storage();
    FlipperFormat* fff_file = flipper_format_file_alloc(storage);

    if(storage_common_stat(storage, HEX_VIEWER_SETTINGS_SAVE_PATH, NULL) != FSE_OK) {
        hex_viewer_close_config_file(fff_file);
        hex_viewer_close_storage();
        return;
    }
    uint32_t file_version;
    FuriString* temp_str = furi_string_alloc();

    if(!flipper_format_file_open_existing(fff_file, HEX_VIEWER_SETTINGS_SAVE_PATH)) {
        FURI_LOG_E(TAG, "Cannot open file %s", HEX_VIEWER_SETTINGS_SAVE_PATH);
        hex_viewer_close_config_file(fff_file);
        hex_viewer_close_storage();
        return;
    }

    if(!flipper_format_read_header(fff_file, temp_str, &file_version)) {
        FURI_LOG_E(TAG, "Missing Header Data");
        hex_viewer_close_config_file(fff_file);
        hex_viewer_close_storage();
        return;
    }

    if(file_version < HEX_VIEWER_SETTINGS_FILE_VERSION) {
        FURI_LOG_I(TAG, "old config version, will be removed.");
        hex_viewer_close_config_file(fff_file);
        hex_viewer_close_storage();
        return;
    }

    flipper_format_read_uint32(fff_file, HEX_VIEWER_SETTINGS_KEY_HAPTIC, &app->haptic, 1);
    flipper_format_read_uint32(fff_file, HEX_VIEWER_SETTINGS_KEY_SPEAKER, &app->speaker, 1);
    flipper_format_read_uint32(fff_file, HEX_VIEWER_SETTINGS_KEY_LED, &app->led, 1);
    flipper_format_read_uint32(
        fff_file, HEX_VIEWER_SETTINGS_KEY_SAVE_SETTINGS, &app->save_settings, 1);

    flipper_format_rewind(fff_file);

    hex_viewer_close_config_file(fff_file);
    hex_viewer_close_storage();
}

bool hex_viewer_open_file(void* context, const char* file_path) {
    HexViewer* hex_viewer = context;
    furi_assert(hex_viewer);
    furi_assert(file_path);

    // TODO Separate function?
    if(hex_viewer->model->stream) {
        buffered_file_stream_close(hex_viewer->model->stream);
        stream_free(hex_viewer->model->stream);
        hex_viewer->model->file_offset = 0;
    }

    hex_viewer->model->stream = buffered_file_stream_alloc(hex_viewer->storage);
    bool isOk = true;

    do {
        if(!buffered_file_stream_open(
               hex_viewer->model->stream, file_path, FSAM_READ, FSOM_OPEN_EXISTING)) {
            FURI_LOG_E(TAG, "Unable to open stream: %s", file_path);
            isOk = false;
            break;
        };

        hex_viewer->model->file_size = stream_size(hex_viewer->model->stream);
    } while(false);

    return isOk;
}

bool hex_viewer_read_file(void* context) {
    HexViewer* hex_viewer = context;
    furi_assert(hex_viewer);
    furi_assert(hex_viewer->model->stream);
    furi_assert(hex_viewer->model->file_offset % HEX_VIEWER_BYTES_PER_LINE == 0);

    memset(hex_viewer->model->file_bytes, 0x0, HEX_VIEWER_BUF_SIZE);
    bool isOk = true;

    do {
        uint32_t offset = hex_viewer->model->file_offset;
        if(!stream_seek(hex_viewer->model->stream, offset, true)) {
            FURI_LOG_E(TAG, "Unable to seek stream");
            isOk = false;
            break;
        }

        hex_viewer->model->file_read_bytes = stream_read(
            hex_viewer->model->stream,
            (uint8_t*)hex_viewer->model->file_bytes,
            HEX_VIEWER_BUF_SIZE);
    } while(false);

    return isOk;
}