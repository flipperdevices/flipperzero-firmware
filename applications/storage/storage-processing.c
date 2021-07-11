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
        StorageData* ext_storage = &app->storage[ST_EXT];

        if(storage_data_status(ext_storage) == SE_OK) {
            type = ST_EXT;
        }
    }
    storage = &app->storage[type];

    return storage;
}

static bool storage_type_is_not_valid(StorageType type) {
    return type >= ST_ERROR;
}

static StorageData* get_storage_by_file(File* file, StorageData* storages) {
    StorageData* storage_data = NULL;

    for(uint8_t i = 0; i < STORAGE_COUNT; i++) {
        if(storage_has_file(file, &storages[i])) {
            storage_data = &storages[i];
        }
    }

    furi_check(storage_data != NULL);

    return storage_data;
}

const char* remove_vfs(const char* path) {
    if(strlen(path) < 4) {
        furi_check(0);
    }

    return path + 4;
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
    } else {
        storage = storage_get_storage_by_type(app, type);
        if(storage_path_already_open(path, storage->files)) {
            file->error_id = FSE_ALREADY_OPEN;
        } else {
            storage_push_storage_file(file, path, type, storage);
            FS_CALL(storage, file.open(storage, file, remove_vfs(path), access_mode, open_mode));
        }
    }

    return ret;
}

bool storage_process_close(StorageApp* app, File* file) {
    bool ret = false;
    StorageData* storage = get_storage_by_file(file, app->storage);
    FS_CALL(storage, file.close(storage, file));
    storage_pop_storage_file(file, storage);
    return ret;
}

static uint16_t
    storage_sd_file_read(StorageApp* app, File* file, void* buff, uint16_t const bytes_to_read) {
    uint16_t ret = 0;
    StorageData* storage = get_storage_by_file(file, app->storage);
    FS_CALL(storage, file.read(storage, file, buff, bytes_to_read));
    return ret;
}

static uint16_t storage_sd_file_write(
    StorageApp* app,
    File* file,
    const void* buff,
    uint16_t const bytes_to_write) {
    uint16_t ret = 0;
    StorageData* storage = get_storage_by_file(file, app->storage);
    FS_CALL(storage, file.write(storage, file, buff, bytes_to_write));
    return ret;
}

void storage_process_message(StorageApp* app, StorageMessage* message) {
    switch(message->command) {
    case StorageCommandFileOpen:
        message->return_data->bool_value = storage_process_open(
            app,
            message->data->fopen.file,
            message->data->fopen.path,
            message->data->fopen.access_mode,
            message->data->fopen.open_mode);
        break;
    case StorageCommandFileClose:
        message->return_data->bool_value = storage_process_close(app, message->data->fopen.file);
        break;
    case StorageCommandFileRead:
        message->return_data->uint16_value = storage_sd_file_read(
            app,
            message->data->fread.file,
            message->data->fread.buff,
            message->data->fread.bytes_to_read);
        break;
    case StorageCommandFileWrite:
        message->return_data->uint16_value = storage_sd_file_write(
            app,
            message->data->fwrite.file,
            message->data->fwrite.buff,
            message->data->fwrite.bytes_to_write);
        break;
    default:
        break;
    }

    osThreadFlagsSet(message->thread, STORAGE_THREAD_FLAG_COMPLETE);
}
