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
    furi_check(storage->mutex != NULL);
    storage->data = NULL;
    storage->status = StorageStatusNotReady;
    StorageFileList_init(storage->files);
}

bool storage_data_lock(StorageData* storage) {
    api_hal_power_insomnia_enter();
    return (osMutexAcquire(storage->mutex, osWaitForever) == osOK);
}

bool storage_data_unlock(StorageData* storage) {
    api_hal_power_insomnia_exit();
    return (osMutexRelease(storage->mutex) == osOK);
}

StorageStatus storage_data_status(StorageData* storage) {
    StorageStatus status;

    storage_data_lock(storage);
    status = storage->status;
    storage_data_unlock(storage);

    return status;
}

const char* storage_data_status_text(StorageData* storage) {
    const char* result = "unknown";
    switch(storage->status) {
    case StorageStatusOK:
        result = "ok";
        break;
    case StorageStatusNotReady:
        result = "not ready";
        break;
    case StorageStatusNotMounted:
        result = "not mounted";
        break;
    case StorageStatusNoFS:
        result = "no filesystem";
        break;
    case StorageStatusNotAccessible:
        result = "not accessible";
        break;
    case StorageStatusErrorInternal:
        result = "internal";
        break;
    }

    return result;
}

/****************** helpers ******************/

static void check_path(const char* path) {
    if(strlen(path) == 0) {
        furi_check(0);
    }

    if(path[0] != '/') {
        furi_check(0);
    }
}

/****************** storage glue ******************/

bool storage_has_file(const File* file, StorageData* storage_data) {
    bool result = false;

    StorageFileList_it_t it;
    for(StorageFileList_it(it, storage_data->files); !StorageFileList_end_p(it);
        StorageFileList_next(it)) {
        const StorageFile* storage_file = StorageFileList_cref(it);

        if(storage_file->file->file_id == file->file_id) {
            result = true;
            break;
        }
    }

    return result;
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

bool storage_path_already_open(const char* path, StorageFileList_t array) {
    bool open = false;
    check_path(path);

    StorageFileList_it_t it;

    for(StorageFileList_it(it, array); !StorageFileList_end_p(it); StorageFileList_next(it)) {
        const StorageFile* storage_file = StorageFileList_cref(it);

        if(string_cmp(storage_file->path, path) == 0) {
            open = true;
            break;
        }
    }

    return open;
}

void storage_set_storage_file_data(const File* file, void* file_data, StorageData* storage) {
    StorageFile* founded_file = NULL;

    StorageFileList_it_t it;

    for(StorageFileList_it(it, storage->files); !StorageFileList_end_p(it);
        StorageFileList_next(it)) {
        StorageFile* storage_file = StorageFileList_ref(it);

        if(storage_file->file->file_id == file->file_id) {
            founded_file = storage_file;
            break;
        }
    }

    furi_check(founded_file != NULL);

    founded_file->file_data = file_data;
}

void* storage_get_storage_file_data(const File* file, StorageData* storage) {
    const StorageFile* founded_file = NULL;

    StorageFileList_it_t it;

    for(StorageFileList_it(it, storage->files); !StorageFileList_end_p(it);
        StorageFileList_next(it)) {
        const StorageFile* storage_file = StorageFileList_cref(it);

        if(storage_file->file->file_id == file->file_id) {
            founded_file = storage_file;
            break;
        }
    }

    furi_check(founded_file != NULL);

    return founded_file->file_data;
}

void storage_push_storage_file(
    File* file,
    const char* path,
    StorageType type,
    StorageData* storage) {
    StorageFile* storage_file = StorageFileList_push_new(storage->files);
    furi_check(storage_file != NULL);

    file->file_id = (uint32_t)storage_file;
    storage_file->file = file;
    storage_file->type = type;
    string_set(storage_file->path, path);
}

bool storage_pop_storage_file(File* file, StorageData* storage) {
    StorageFileList_it_t it;
    bool result = false;

    for(StorageFileList_it(it, storage->files); !StorageFileList_end_p(it);
        StorageFileList_next(it)) {
        if(StorageFileList_cref(it)->file->file_id == file->file_id) {
            result = true;
            break;
        }
    }

    if(result) {
        StorageFileList_remove(storage->files, it);
    }

    return result;
}
