#include "application_assets.h"
#include <toolbox/path.h>

#define FLIPPER_APPLICATION_ASSETS_MAGIC 0x4F4C5A44
#define FLIPPER_APPLICATION_ASSETS_VERSION 0x00000001

#define TAG "fap_assets"

#pragma pack(push, 1)

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t dirs_count;
    uint32_t files_count;
} FlipperApplicationAssetsHeader;

#pragma pack(pop)

bool flipper_application_assets_load(File* file, const char* elf_path, size_t offset, size_t size) {
    UNUSED(size);
    FlipperApplicationAssetsHeader header;
    bool result = false;

    FuriString* app_name = furi_string_alloc();
    path_extract_filename_no_ext(elf_path, app_name);

    FURI_LOG_I(TAG, "Loading assets for %s", furi_string_get_cstr(app_name));

    do {
        if(!storage_file_seek(file, offset, true)) {
            break;
        }

        // read header
        if(storage_file_read(file, &header, sizeof(header)) != sizeof(header)) {
            break;
        }

        if(header.magic != FLIPPER_APPLICATION_ASSETS_MAGIC) {
            break;
        }

        if(header.version != FLIPPER_APPLICATION_ASSETS_VERSION) {
            break;
        }

        uint32_t length = 0;
        char* path = NULL;

        FURI_LOG_I(TAG, "Creating %ld directories", header.dirs_count);
        for(uint32_t i = 0; i < header.dirs_count; i++) {
            // read path length
            if(storage_file_read(file, &length, sizeof(length)) != sizeof(length)) {
                break;
            }

            path = malloc(length + 1);

            // read path
            if(storage_file_read(file, path, length) != length) {
                break;
            }

            path[length] = '\0';

            FURI_LOG_I(TAG, "Creating directory: %s", path);

            free(path);
            path = NULL;
        }

        FURI_LOG_I(TAG, "Creating %ld files", header.files_count);
        for(uint32_t i = 0; i < header.files_count; i++) {
            // read path length
            if(storage_file_read(file, &length, sizeof(length)) != sizeof(length)) {
                break;
            }

            path = malloc(length + 1);

            // read path
            if(storage_file_read(file, path, length) != length) {
                break;
            }

            path[length] = '\0';

            // read file size
            if(storage_file_read(file, &length, sizeof(length)) != sizeof(length)) {
                break;
            }

            // mock for reading file content
            if(!storage_file_seek(file, length, false)) {
                break;
            }

            FURI_LOG_I(TAG, "Creating file: %s, size: %ld", path, length);

            free(path);
            path = NULL;
        }

        result = true;
    } while(false);

    furi_string_free(app_name);

    return result;
}