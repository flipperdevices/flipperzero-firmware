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

/******************* File Functions *******************/

bool storage_process_file_open(
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

bool storage_process_file_close(StorageApp* app, File* file) {
    bool ret = false;
    StorageData* storage = get_storage_by_file(file, app->storage);
    FS_CALL(storage, file.close(storage, file));
    storage_pop_storage_file(file, storage);
    return ret;
}

static uint16_t storage_process_file_read(
    StorageApp* app,
    File* file,
    void* buff,
    uint16_t const bytes_to_read) {
    uint16_t ret = 0;
    StorageData* storage = get_storage_by_file(file, app->storage);
    FS_CALL(storage, file.read(storage, file, buff, bytes_to_read));
    return ret;
}

static uint16_t storage_process_file_write(
    StorageApp* app,
    File* file,
    const void* buff,
    uint16_t const bytes_to_write) {
    uint16_t ret = 0;
    StorageData* storage = get_storage_by_file(file, app->storage);
    FS_CALL(storage, file.write(storage, file, buff, bytes_to_write));
    return ret;
}

static bool storage_process_file_seek(
    StorageApp* app,
    File* file,
    const uint32_t offset,
    const bool from_start) {
    bool ret = false;
    StorageData* storage = get_storage_by_file(file, app->storage);
    FS_CALL(storage, file.seek(storage, file, offset, from_start));
    return ret;
}

static uint64_t storage_process_file_tell(StorageApp* app, File* file) {
    uint64_t ret = 0;
    StorageData* storage = get_storage_by_file(file, app->storage);
    FS_CALL(storage, file.tell(storage, file));
    return ret;
}

static bool storage_process_file_truncate(StorageApp* app, File* file) {
    bool ret = false;
    StorageData* storage = get_storage_by_file(file, app->storage);
    FS_CALL(storage, file.truncate(storage, file));
    return ret;
}

static bool storage_process_file_sync(StorageApp* app, File* file) {
    bool ret = false;
    StorageData* storage = get_storage_by_file(file, app->storage);
    FS_CALL(storage, file.sync(storage, file));
    return ret;
}

static uint64_t storage_process_file_size(StorageApp* app, File* file) {
    uint64_t ret = 0;
    StorageData* storage = get_storage_by_file(file, app->storage);
    FS_CALL(storage, file.size(storage, file));
    return ret;
}

static bool storage_process_file_eof(StorageApp* app, File* file) {
    bool ret = false;
    StorageData* storage = get_storage_by_file(file, app->storage);
    FS_CALL(storage, file.eof(storage, file));
    return ret;
}

/******************* Dir Functions *******************/

// storage_process_dir_read storage_process_dir_rewind

bool storage_process_dir_open(StorageApp* app, File* file, const char* path) {
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
            FS_CALL(storage, dir.open(storage, file, remove_vfs(path)));
        }
    }

    return ret;
}

bool storage_process_dir_close(StorageApp* app, File* file) {
    bool ret = false;
    StorageData* storage = get_storage_by_file(file, app->storage);
    FS_CALL(storage, dir.close(storage, file));
    storage_pop_storage_file(file, storage);
    return ret;
}

bool storage_process_dir_read(
    StorageApp* app,
    File* file,
    FileInfo* fileinfo,
    char* name,
    const uint16_t name_length) {
    bool ret = false;
    StorageData* storage = get_storage_by_file(file, app->storage);
    FS_CALL(storage, dir.read(storage, file, fileinfo, name, name_length));
    return ret;
}

bool storage_process_dir_rewind(StorageApp* app, File* file) {
    bool ret = false;
    StorageData* storage = get_storage_by_file(file, app->storage);
    FS_CALL(storage, dir.rewind(storage, file));
    return ret;
}

/******************* Common FS Functions *******************/

static FS_Error
    storage_process_common_stat(StorageApp* app, const char* path, FileInfo* fileinfo) {
    FS_Error ret = FSE_OK;
    StorageType type = storage_get_type_by_path(path);

    if(storage_type_is_not_valid(type)) {
        ret = FSE_INVALID_NAME;
    } else {
        StorageData* storage = storage_get_storage_by_type(app, type);
        FS_CALL(storage, common.stat(storage, remove_vfs(path), fileinfo));
    }

    return ret;
}

static FS_Error storage_process_common_remove(StorageApp* app, const char* path) {
    FS_Error ret = FSE_OK;
    StorageType type = storage_get_type_by_path(path);

    if(storage_type_is_not_valid(type)) {
        ret = FSE_INVALID_NAME;
    } else {
        StorageData* storage = storage_get_storage_by_type(app, type);
        FS_CALL(storage, common.remove(storage, remove_vfs(path)));
    }

    return ret;
}

static FS_Error
    storage_process_common_rename(StorageApp* app, const char* old_path, const char* new_path) {
    FS_Error ret = FSE_OK;
    StorageType type_old = storage_get_type_by_path(old_path);
    StorageType type_new = storage_get_type_by_path(new_path);

    if(storage_type_is_not_valid(type_old) || storage_type_is_not_valid(type_old) ||
       type_old != type_new) {
        ret = FSE_INVALID_NAME;
    } else {
        StorageData* storage = storage_get_storage_by_type(app, type_old);
        FS_CALL(storage, common.rename(storage, remove_vfs(old_path), remove_vfs(new_path)));
    }

    return ret;
}

static FS_Error storage_process_common_mkdir(StorageApp* app, const char* path) {
    FS_Error ret = FSE_OK;
    StorageType type = storage_get_type_by_path(path);

    if(storage_type_is_not_valid(type)) {
        ret = FSE_INVALID_NAME;
    } else {
        StorageData* storage = storage_get_storage_by_type(app, type);
        FS_CALL(storage, common.mkdir(storage, remove_vfs(path)));
    }

    return ret;
}

static FS_Error storage_process_common_fs_info(
    StorageApp* app,
    const char* fs_path,
    uint64_t* total_space,
    uint64_t* free_space) {
    FS_Error ret = FSE_OK;
    StorageType type = storage_get_type_by_path(fs_path);

    if(storage_type_is_not_valid(type)) {
        ret = FSE_INVALID_NAME;
    } else {
        StorageData* storage = storage_get_storage_by_type(app, type);
        FS_CALL(storage, common.fs_info(storage, remove_vfs(fs_path), total_space, free_space));
    }

    return ret;
}

/******************* Error Reporting Functions *******************/

static const char* storage_process_error_get_desc(FS_Error error_id) {
    const char* result = "unknown error";
    switch(error_id) {
    case(FSE_OK):
        result = "OK";
        break;
    case(FSE_NOT_READY):
        result = "filesystem not ready";
        break;
    case(FSE_EXIST):
        result = "file/dir already exist";
        break;
    case(FSE_NOT_EXIST):
        result = "file/dir not exist";
        break;
    case(FSE_INVALID_PARAMETER):
        result = "invalid parameter";
        break;
    case(FSE_DENIED):
        result = "access denied";
        break;
    case(FSE_INVALID_NAME):
        result = "invalid name/path";
        break;
    case(FSE_INTERNAL):
        result = "internal error";
        break;
    case(FSE_NOT_IMPLEMENTED):
        result = "function not implemented";
        break;
    case(FSE_ALREADY_OPEN):
        result = "file is already open";
        break;
    }
    return result;
}

void storage_process_message(StorageApp* app, StorageMessage* message) {
    switch(message->command) {
    case StorageCommandFileOpen:
        message->return_data->bool_value = storage_process_file_open(
            app,
            message->data->fopen.file,
            message->data->fopen.path,
            message->data->fopen.access_mode,
            message->data->fopen.open_mode);
        break;
    case StorageCommandFileClose:
        message->return_data->bool_value =
            storage_process_file_close(app, message->data->fopen.file);
        break;
    case StorageCommandFileRead:
        message->return_data->uint16_value = storage_process_file_read(
            app,
            message->data->fread.file,
            message->data->fread.buff,
            message->data->fread.bytes_to_read);
        break;
    case StorageCommandFileWrite:
        message->return_data->uint16_value = storage_process_file_write(
            app,
            message->data->fwrite.file,
            message->data->fwrite.buff,
            message->data->fwrite.bytes_to_write);
        break;
    case StorageCommandFileSeek:
        message->return_data->bool_value = storage_process_file_seek(
            app,
            message->data->fseek.file,
            message->data->fseek.offset,
            message->data->fseek.from_start);
        break;
    case StorageCommandFileTell:
        message->return_data->uint64_value =
            storage_process_file_tell(app, message->data->file.file);
        break;
    case StorageCommandFileTruncate:
        message->return_data->bool_value =
            storage_process_file_truncate(app, message->data->file.file);
        break;
    case StorageCommandFileSync:
        message->return_data->bool_value =
            storage_process_file_sync(app, message->data->file.file);
        break;
    case StorageCommandFileSize:
        message->return_data->uint64_value =
            storage_process_file_size(app, message->data->file.file);
        break;
    case StorageCommandFileEof:
        message->return_data->bool_value = storage_process_file_eof(app, message->data->file.file);
        break;

    case StorageCommandDirOpen:
        message->return_data->bool_value =
            storage_process_dir_open(app, message->data->dopen.file, message->data->dopen.path);
        break;
    case StorageCommandDirClose:
        message->return_data->bool_value =
            storage_process_dir_close(app, message->data->file.file);
        break;
    case StorageCommandDirRead:
        message->return_data->bool_value = storage_process_dir_read(
            app,
            message->data->dread.file,
            message->data->dread.fileinfo,
            message->data->dread.name,
            message->data->dread.name_length);
        break;
    case StorageCommandDirRewind:
        message->return_data->bool_value =
            storage_process_dir_rewind(app, message->data->file.file);
        break;
    case StorageCommandCommonStat:
        message->return_data->error_value = storage_process_common_stat(
            app, message->data->cstat.path, message->data->cstat.fileinfo);
        break;
    case StorageCommandCommonRemove:
        message->return_data->error_value =
            storage_process_common_remove(app, message->data->path.path);
        break;
    case StorageCommandCommonRename:
        message->return_data->error_value = storage_process_common_rename(
            app, message->data->crename.old_path, message->data->crename.new_path);
        break;
    case StorageCommandCommonMkDir:
        message->return_data->error_value =
            storage_process_common_mkdir(app, message->data->path.path);
        break;
    case StorageCommandCommonFSInfo:
        message->return_data->error_value = storage_process_common_fs_info(
            app,
            message->data->cfsinfo.fs_path,
            message->data->cfsinfo.total_space,
            message->data->cfsinfo.free_space);
        break;
    case StorageCommandErrorGetDesc:
        message->return_data->cstring_value =
            storage_process_error_get_desc(message->data->error.id);
        break;
    }

    osThreadFlagsSet(message->thread, STORAGE_THREAD_FLAG_COMPLETE);
}
