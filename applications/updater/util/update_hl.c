#include "update_hl.h"

#include "update_manifest.h"

#include <furi.h>
#include <furi_hal.h>
#include <m-string.h>
#include <storage/storage.h>
#include <loader/loader.h>

// TODO: dedup code with updater_cli_apply?..
bool update_hl_prepare(const char* manifest_dir_path) {
    const char* final_manifest_dir_path = manifest_dir_path && strlen(manifest_dir_path) ?
                                              manifest_dir_path :
                                              "/ext" UPDATE_DIR_DEFAULT_REL_PATH;

    string_t full_manifest_path;
    string_init_printf(
        full_manifest_path, "%s/%s", final_manifest_dir_path, UPDATE_MANIFEST_DEFAULT_NAME);

    bool result = false;
    UpdateManifest* manifest = update_manifest_alloc();
    if(update_manifest_init(manifest, string_get_cstr(full_manifest_path))) {
        Storage* storage = furi_record_open("storage");
        File* file = storage_file_alloc(storage);

        string_t abs_path;
        string_init_printf(
            abs_path,
            "%s/%s",
            final_manifest_dir_path,
            string_get_cstr(manifest->staged_loader_file));

        const uint16_t READ_BLOCK = 0x1000;
        uint8_t* read_buffer = malloc(READ_BLOCK);
        uint32_t crc = 0;
        do {
            if(!storage_file_open(file, string_get_cstr(abs_path), FSAM_READ, FSOM_OPEN_EXISTING)) {
                break;
            }

            furi_hal_crc_acquire(osWaitForever);

            uint16_t bytes_read = 0;
            do {
                bytes_read = storage_file_read(file, read_buffer, READ_BLOCK);
                crc = furi_hal_crc_feed(read_buffer, bytes_read);
            } while(bytes_read == READ_BLOCK);

            furi_hal_crc_reset();
        } while(false);

        string_clear(abs_path);
        free(read_buffer);
        storage_file_free(file);
        furi_record_close("storage");

        if(crc == manifest->staged_loader_crc) {
            furi_hal_rtc_set_flag(FuriHalRtcFlagExecutePreUpdate);
            result = true;
        }
    }
    string_clear(full_manifest_path);
    update_manifest_free(manifest);

    return result;
}

bool update_hl_is_armed() {
    return furi_hal_rtc_is_flag_set(FuriHalRtcFlagExecutePreUpdate);
}

void update_hl_disarm() {
    furi_hal_rtc_reset_flag(FuriHalRtcFlagExecutePreUpdate);
}