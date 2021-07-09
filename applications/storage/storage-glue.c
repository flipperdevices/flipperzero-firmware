#include "storage-glue.h"
#include <api-hal.h>

/****************** storage file ******************/

void storage_file_init(StorageFile* obj) {
    obj->file = NULL;
    obj->type = ST_ERROR;
    obj->file_data = NULL;
    string_init(obj->path);
}

void storage_file_init_set(StorageFile* obj, const StorageFile* src) {
    obj->file = src->file;
    obj->type = src->type;
    obj->file_data = src->file_data;
    string_init_set(obj->path, src->path);
}

void storage_file_set(StorageFile* obj, const StorageFile* src) {
    obj->file = src->file;
    obj->type = src->type;
    obj->file_data = src->file_data;
    string_set(obj->path, src->path);
}

void storage_file_clear(StorageFile* obj) {
    string_clear(obj->path);
}

/****************** storage data ******************/

void storage_data_init(StorageData* storage) {
    storage->mutex = osMutexNew(NULL);
    if(storage->mutex == NULL) furi_check(false);
    storage->data = NULL;
}

bool storage_data_lock(StorageData* storage) {
    api_hal_power_insomnia_enter();
    return (osMutexAcquire(storage->mutex, osWaitForever) == osOK);
}

bool storage_data_unlock(StorageData* storage) {
    api_hal_power_insomnia_exit();
    return (osMutexRelease(storage->mutex) == osOK);
}

/****************** helpers ******************/

static void check_path(const char* path) {
    if(path[0] != '/' || strlen(path) == 0) {
        furi_check(0);
    }
}

/****************** storage glue ******************/

StorageType storage_get_type_by_file(const File* file, StorageFileArray_t* array) {
    StorageType type = ST_ERROR;

    StorageFileArray_it_t it;

    for(StorageFileArray_it(it, *array); !StorageFileArray_end_p(it); StorageFileArray_next(it)) {
        const StorageFile* storage_file = StorageFileArray_cref(it);

        if(storage_file->file->file_id == file->file_id) {
            type = storage_file->type;
            break;
        }
    }

    return type;
}

StorageType storage_get_type_by_path(const char* path) {
    StorageType type = ST_ERROR;
    check_path(path);
    path++;

    const char* sd_path = "ext";
    const char* internal_path = "int";
    const char* user_path = "any";

    if(strlen(path) >= strlen(sd_path) && memcmp(path, sd_path, strlen(sd_path)) == 0) {
        type = ST_EXT;
    } else if(
        strlen(path) >= strlen(internal_path) &&
        memcmp(path, internal_path, strlen(internal_path)) == 0) {
        type = ST_INT;
    } else if(strlen(path) >= strlen(user_path) && memcmp(path, user_path, strlen(user_path)) == 0) {
        type = ST_ANY;
    }

    return type;
}

bool storage_path_already_open(const char* path, StorageFileArray_t* array) {
    bool open = false;
    check_path(path);

    StorageFileArray_it_t it;

    for(StorageFileArray_it(it, *array); !StorageFileArray_end_p(it); StorageFileArray_next(it)) {
        const StorageFile* storage_file = StorageFileArray_cref(it);

        if(string_cmp(storage_file->path, path) == 0) {
            open = true;
            break;
        }
    }

    return open;
}

StorageFile* storage_get_storage_file(const File* file, StorageFileArray_t* array) {
    StorageFile* founded_file = NULL;

    StorageFileArray_it_t it;

    for(StorageFileArray_it(it, *array); !StorageFileArray_end_p(it); StorageFileArray_next(it)) {
        StorageFile* storage_file = StorageFileArray_ref(it);

        if(storage_file->file->file_id == file->file_id) {
            founded_file = storage_file;
            break;
        }
    }

    return founded_file;
}
