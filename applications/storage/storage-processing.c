#include "storage-processing.h"

#define FS_CALL(_storage, _fn)   \
    storage_data_lock(_storage); \
    ret = _storage->fs_api._fn;  \
    storage_data_unlock(_storage);

#define ST_CALL(_storage, _fn)   \
    storage_data_lock(_storage); \
    ret = _storage->api._fn;     \
    storage_data_unlock(_storage);

static StorageData* storage_get_storage_by_type(StorageApp* app, StorageType type) {
    StorageData* storage;

    if(type == ST_ANY) {
        type = ST_INT;
        FS_Error ret;
        StorageData* ext_storage = &app->storage[ST_EXT];
        ST_CALL(ext_storage, status(ext_storage));

        if(ret == FSE_OK) {
            type = ST_EXT;
        }
    }
    storage = &app->storage[type];

    return storage;
}

static bool storage_type_is_not_valid(StorageType type) {
    return type >= ST_ERROR;
}

bool storage_process_open(
    StorageApp* app,
    File* file,
    const char* path,
    FS_AccessMode access_mode,
    FS_OpenMode open_mode) {
    bool ret = false;
    StorageType type = storage_get_type_by_path(path);
    StorageData* storage;
    file->error_id = FSE_OK;

    if(storage_type_is_not_valid(type)) {
        file->error_id = FSE_INVALID_NAME;
    }

    if(storage_path_already_open(path, &app->files)) {
        file->error_id = FSE_ALREADY_OPEN;
    }

    if(file->error_id == FSE_OK) {
        storage = storage_get_storage_by_type(app, type);
        FS_CALL(storage, file.open(storage, file, path, access_mode, open_mode));
    }

    return ret;
}

void storage_process_message(StorageApp* app, StorageMessage* message) {
    switch(message->command) {
    case SC_FILE_OPEN:
        message->return_data->bool_value = storage_process_open(
            app,
            message->data->fopen.file,
            message->data->fopen.path,
            message->data->fopen.access_mode,
            message->data->fopen.open_mode);
        break;
    }

    osThreadFlagsSet(message->thread, STORAGE_THREAD_FLAG_COMPLETE);
}
