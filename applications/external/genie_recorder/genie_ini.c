#include "genie_ini.h"
#include "genie_app.h"

#include <furi.h>
#include <flipper_format.h>

#define GENIE_SETTINGS_FILE EXT_PATH("apps_data") "/genie/genie.txt"
#define GENIE_SETTINGS_NAME "Genie settings file"
#define GENIE_SETTINGS_VERSION 1

#define TAG "GenieIni"

FlipperFormat* ff;

void genie_ini_load(GenieApp* app) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FuriString* buf = furi_string_alloc();

    ff = flipper_format_buffered_file_alloc(storage);
    do {
        uint32_t format_version;
        if(!flipper_format_buffered_file_open_existing(ff, GENIE_SETTINGS_FILE)) {
            FURI_LOG_E(TAG, "Failed to open settings file: %s", GENIE_SETTINGS_FILE);
            break;
        }
        if(!flipper_format_read_header(ff, buf, &format_version)) {
            FURI_LOG_E(TAG, "Failed to read settings header.");
            break;
        }
        if(furi_string_cmp_str(buf, GENIE_SETTINGS_NAME) ||
           format_version != GENIE_SETTINGS_VERSION) {
            FURI_LOG_E(
                TAG, "Unsupported file.  `%s` v`%ld`", furi_string_get_cstr(buf), format_version);
            break;
        }

        uint32_t frequency;
        flipper_format_read_uint32(ff, "frequency", &frequency, 1);
        genie_app_set_frequency(app, frequency);

        FuriString* buf = furi_string_alloc();
        if(flipper_format_read_string(ff, "path", buf)) {
            genie_app_update_file_path(app, furi_string_get_cstr(buf));
        }
    } while(false);

    flipper_format_buffered_file_close(ff);
    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);
    furi_string_free(buf);
}

void genie_ini_save(GenieApp* app) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FuriString* buf = furi_string_alloc();

    ff = flipper_format_buffered_file_alloc(storage);
    do {
        if(!flipper_format_buffered_file_open_always(ff, GENIE_SETTINGS_FILE)) {
            FURI_LOG_E(TAG, "Failed to open settings file: %s", GENIE_SETTINGS_FILE);
            break;
        }
        if(!flipper_format_write_header_cstr(ff, GENIE_SETTINGS_NAME, GENIE_SETTINGS_VERSION)) {
            FURI_LOG_E(TAG, "Failed to write settings header.");
            break;
        }

        uint32_t frequency = genie_app_get_frequency(app);
        flipper_format_write_uint32(ff, "frequency", &frequency, 1);

        const char* path = genie_app_get_file_path(app);
        flipper_format_write_string_cstr(ff, "path", path);
    } while(false);

    flipper_format_buffered_file_close(ff);
    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);
    furi_string_free(buf);
}
