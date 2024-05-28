#include "fxbm.h"
#include <storage/storage.h>

struct FxbmFile {
    uint32_t width;
    uint32_t height;
    uint8_t data[];
};

FxbmFile* fxbm_alloc(const char* path) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);
    FxbmFile* fxbm = NULL;
    uint32_t size = sizeof(size);

    do {
        if(!storage_file_open(file, path, FSAM_READ, FSOM_OPEN_EXISTING)) {
            break;
        }

        if(storage_file_read(file, &size, size) != sizeof(size)) {
            break;
        }

        fxbm = malloc(size);
        if(storage_file_read(file, fxbm, size) != size) {
            free(fxbm);
            fxbm = NULL;
            break;
        }
    } while(false);

    storage_file_free(file);
    return fxbm;
}

void fxbm_free(FxbmFile* fxbm) {
    free(fxbm);
}

uint32_t fxbm_get_width(FxbmFile* fxbm) {
    return fxbm->width;
}

uint32_t fxbm_get_height(FxbmFile* fxbm) {
    return fxbm->height;
}

uint8_t* fxbm_get_data(FxbmFile* fxbm) {
    return fxbm->data;
}
