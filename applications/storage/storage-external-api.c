#include "storage.h"
#include "storage-i.h"
#include "storage-message.h"

#define S_API_PROLOGUE                            \
    osThreadId_t caller_thread = osThreadGetId(); \
    if(caller_thread == 0) furi_check(0);

#define S_API_EPILOGUE                                                                     \
    furi_check(osMessageQueuePut(app->message_queue, &message, 0, osWaitForever) == osOK); \
    osThreadFlagsWait(STORAGE_THREAD_FLAG_COMPLETE, osFlagsWaitAny, osWaitForever);

#define S_API_MESSAGE(_command)      \
    SAReturn return_data;            \
    StorageMessage message = {       \
        .thread = caller_thread,     \
        .command = _command,         \
        .data = &data,               \
        .return_data = &return_data, \
    };

#define S_API_DATA_FILE   \
    SAData data = {       \
        .file = {         \
            .file = file, \
        }};

#define S_API_DATA_PATH   \
    SAData data = {       \
        .path = {         \
            .path = path, \
        }};

#define S_RETURN_BOOL (return_data.bool_value);
#define S_RETURN_UINT16 (return_data.uint16_value);
#define S_RETURN_UINT64 (return_data.uint64_value);
#define S_RETURN_ERROR (return_data.error_value);
#define S_RETURN_CSTRING (return_data.cstring_value);

/****************** FILE ******************/

bool storage_file_open(
    StorageApp* app,
    File* file,
    const char* path,
    FS_AccessMode access_mode,
    FS_OpenMode open_mode) {
    S_API_PROLOGUE;

    SAData data = {
        .fopen = {
            .file = file,
            .path = path,
            .access_mode = access_mode,
            .open_mode = open_mode,
        }};

    S_API_MESSAGE(StorageCommandFileOpen);
    S_API_EPILOGUE;
    return S_RETURN_BOOL;
}

bool storage_file_close(StorageApp* app, File* file) {
    S_API_PROLOGUE;
    S_API_DATA_FILE;
    S_API_MESSAGE(StorageCommandFileClose);
    S_API_EPILOGUE;
    return S_RETURN_BOOL;
}

uint16_t storage_file_read(StorageApp* app, File* file, void* buff, uint16_t bytes_to_read) {
    S_API_PROLOGUE;

    SAData data = {
        .fread = {
            .file = file,
            .buff = buff,
            .bytes_to_read = bytes_to_read,
        }};

    S_API_MESSAGE(StorageCommandFileRead);
    S_API_EPILOGUE;
    return S_RETURN_UINT16;
}

uint16_t storage_file_write(StorageApp* app, File* file, const void* buff, uint16_t bytes_to_write) {
    S_API_PROLOGUE;

    SAData data = {
        .fwrite = {
            .file = file,
            .buff = buff,
            .bytes_to_write = bytes_to_write,
        }};

    S_API_MESSAGE(StorageCommandFileWrite);
    S_API_EPILOGUE;
    return S_RETURN_UINT16;
}

bool storage_file_seek(StorageApp* app, File* file, uint32_t offset, bool from_start) {
    S_API_PROLOGUE;

    SAData data = {
        .fseek = {
            .file = file,
            .offset = offset,
            .from_start = from_start,
        }};

    S_API_MESSAGE(StorageCommandFileSeek);
    S_API_EPILOGUE;
    return S_RETURN_BOOL;
}

uint64_t storage_file_tell(StorageApp* app, File* file) {
    S_API_PROLOGUE;
    S_API_DATA_FILE;
    S_API_MESSAGE(StorageCommandFileTell);
    S_API_EPILOGUE;
    return S_RETURN_UINT64;
}

bool storage_file_truncate(StorageApp* app, File* file) {
    S_API_PROLOGUE;
    S_API_DATA_FILE;
    S_API_MESSAGE(StorageCommandFileTruncate);
    S_API_EPILOGUE;
    return S_RETURN_BOOL;
}

uint64_t storage_file_size(StorageApp* app, File* file) {
    S_API_PROLOGUE;
    S_API_DATA_FILE;
    S_API_MESSAGE(StorageCommandFileSize);
    S_API_EPILOGUE;
    return S_RETURN_UINT64;
}

bool storage_file_sync(StorageApp* app, File* file) {
    S_API_PROLOGUE;
    S_API_DATA_FILE;
    S_API_MESSAGE(StorageCommandFileSync);
    S_API_EPILOGUE;
    return S_RETURN_BOOL;
}

bool storage_file_eof(StorageApp* app, File* file) {
    S_API_PROLOGUE;
    S_API_DATA_FILE;
    S_API_MESSAGE(StorageCommandFileEof);
    S_API_EPILOGUE;
    return S_RETURN_BOOL;
}

/****************** DIR ******************/

bool storage_dir_open(StorageApp* app, File* file, const char* path) {
    S_API_PROLOGUE;

    SAData data = {
        .dopen = {
            .file = file,
            .path = path,
        }};

    S_API_MESSAGE(StorageCommandDirOpen);
    S_API_EPILOGUE;
    return S_RETURN_BOOL;
}

bool storage_dir_close(StorageApp* app, File* file) {
    S_API_PROLOGUE;
    S_API_DATA_FILE;
    S_API_MESSAGE(StorageCommandDirClose);
    S_API_EPILOGUE;
    return S_RETURN_BOOL;
}

bool storage_dir_read(
    StorageApp* app,
    File* file,
    FileInfo* fileinfo,
    char* name,
    uint16_t name_length) {
    S_API_PROLOGUE;

    SAData data = {
        .dread = {
            .file = file,
            .fileinfo = fileinfo,
            .name = name,
            .name_length = name_length,
        }};

    S_API_MESSAGE(StorageCommandDirRead);
    S_API_EPILOGUE;
    return S_RETURN_BOOL;
}

bool storage_dir_rewind(StorageApp* app, File* file) {
    S_API_PROLOGUE;
    S_API_DATA_FILE;
    S_API_MESSAGE(StorageCommandDirRewind);
    S_API_EPILOGUE;
    return S_RETURN_BOOL;
}

/****************** COMMON ******************/

FS_Error storage_common_stat(StorageApp* app, const char* path, FileInfo* fileinfo) {
    S_API_PROLOGUE;

    SAData data = {.cstat = {.path = path, .fileinfo = fileinfo}};

    S_API_MESSAGE(StorageCommandCommonStat);
    S_API_EPILOGUE;
    return S_RETURN_ERROR;
}

FS_Error storage_common_remove(StorageApp* app, const char* path) {
    S_API_PROLOGUE;
    S_API_DATA_PATH;
    S_API_MESSAGE(StorageCommandCommonRemove);
    S_API_EPILOGUE;
    return S_RETURN_ERROR;
}

FS_Error storage_common_rename(StorageApp* app, const char* old_path, const char* new_path) {
    S_API_PROLOGUE;

    SAData data = {
        .crename = {
            .old_path = old_path,
            .new_path = new_path,
        }};

    S_API_MESSAGE(StorageCommandCommonRename);
    S_API_EPILOGUE;
    return S_RETURN_ERROR;
}

FS_Error storage_common_mkdir(StorageApp* app, const char* path) {
    S_API_PROLOGUE;
    S_API_DATA_PATH;
    S_API_MESSAGE(StorageCommandCommonMkDir);
    S_API_EPILOGUE;
    return S_RETURN_ERROR;
}

FS_Error storage_common_fs_info(
    StorageApp* app,
    const char* fs_path,
    uint64_t* total_space,
    uint64_t* free_space) {
    S_API_PROLOGUE;

    SAData data = {
        .cfsinfo = {
            .fs_path = fs_path,
            .total_space = total_space,
            .free_space = free_space,
        }};

    S_API_MESSAGE(StorageCommandCommonFSInfo);
    S_API_EPILOGUE;
    return S_RETURN_ERROR;
}

/****************** ERROR ******************/

const char* storage_error_get_desc(StorageApp* app, FS_Error error_id) {
    S_API_PROLOGUE;
    SAData data = {
        .error = {
            .id = error_id,
        }};
    S_API_MESSAGE(StorageCommandErrorGetDesc);
    S_API_EPILOGUE;
    return S_RETURN_CSTRING;
}

/****************** Raw SD API ******************/

FS_Error storage_sd_format(StorageApp* app) {
    S_API_PROLOGUE;
    SAData data = {};
    S_API_MESSAGE(StorageCommandSDFormat);
    S_API_EPILOGUE;
    return S_RETURN_ERROR;
}

FS_Error storage_sd_unmount(StorageApp* app) {
    S_API_PROLOGUE;
    SAData data = {};
    S_API_MESSAGE(StorageCommandSDUnmount);
    S_API_EPILOGUE;
    return S_RETURN_ERROR;
}

FS_Error storage_sd_info(StorageApp* app, SDInfo* info) {
    S_API_PROLOGUE;
    SAData data = {
        .sdinfo = {
            .info = info,
        }};
    S_API_MESSAGE(StorageCommandSDInfo);
    S_API_EPILOGUE;
    return S_RETURN_ERROR;
}

FS_Error storage_sd_status(StorageApp* app) {
    S_API_PROLOGUE;
    SAData data = {};
    S_API_MESSAGE(StorageCommandSDStatus);
    S_API_EPILOGUE;
    return S_RETURN_ERROR;
}