#include "update_manifest.h"

#include <storage/storage.h>
#include <flipper_format/flipper_format.h>
#include <flipper_format/flipper_format_i.h>

UpdateManifest* update_manifest_alloc() {
    UpdateManifest* update_manifest = malloc(sizeof(UpdateManifest));
    string_init(update_manifest->firmware_dfu_image);
    string_init(update_manifest->radio_image);
    string_init(update_manifest->staged_loader_file);
    update_manifest->target = 0;
    update_manifest->valid = false;
    return update_manifest;
}

void update_manifest_free(UpdateManifest* update_manifest) {
    furi_assert(update_manifest);
    string_clear(update_manifest->firmware_dfu_image);
    string_clear(update_manifest->radio_image);
    string_clear(update_manifest->staged_loader_file);
    free(update_manifest);
}

static bool
    update_manifest_init_from_ff(UpdateManifest* update_manifest, FlipperFormat* flipper_file) {
    furi_assert(update_manifest);
    furi_assert(!update_manifest->valid);
    furi_assert(flipper_file);

    string_t filetype;
    uint32_t version = 0;

    string_init(filetype);
    update_manifest->valid = false;

    do {
        if(!flipper_format_read_header(flipper_file, filetype, &version)) {
            break;
        }

        if(!flipper_format_read_uint32(flipper_file, "Target", &update_manifest->target, 1)) {
            break;
        }

        if(!flipper_format_read_string(
               flipper_file, "Loader", update_manifest->staged_loader_file)) {
            break;
        }

        if(!flipper_format_read_uint32(
               flipper_file, "Loader CRC", &update_manifest->staged_loader_crc, 1)) {
            break;
        }

        if(!flipper_format_read_string(
               flipper_file, "Firmware", update_manifest->firmware_dfu_image)) {
            break;
        }

        if(!flipper_format_read_string(flipper_file, "Radio", update_manifest->radio_image)) {
            break;
        }

        update_manifest->valid = true;
    } while(false);

    return update_manifest->valid;
}

bool update_manifest_init(UpdateManifest* update_manifest, const char* manifest_filename) {
    Storage* storage = furi_record_open("storage");
    FlipperFormat* flipper_file = flipper_format_file_alloc(storage);
    if(flipper_format_file_open_existing(flipper_file, manifest_filename)) {
        update_manifest_init_from_ff(update_manifest, flipper_file);
    }

    flipper_format_free(flipper_file);
    furi_record_close("storage");

    return update_manifest->valid;
}

bool update_manifest_init_mem(
    UpdateManifest* update_manifest,
    const uint8_t* manifest_data,
    const uint16_t length) {
    FlipperFormat* flipper_file = flipper_format_string_alloc();
    Stream* sstream = flipper_format_get_raw_stream(flipper_file);

    stream_write(sstream, manifest_data, length);
    stream_seek(sstream, 0, StreamOffsetFromStart);

    update_manifest_init_from_ff(update_manifest, flipper_file);

    flipper_format_free(flipper_file);

    return update_manifest->valid;
}
