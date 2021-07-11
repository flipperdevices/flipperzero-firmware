#include "storage-i.h"
#include "storage-message.h"

#define S_API_PROLOGUE                            \
    StorageApp* app = context;                    \
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

bool s_api_file_open(
    void* context,
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

bool s_api_file_close(void* context, File* file) {
    S_API_PROLOGUE;
    S_API_DATA_FILE;
    S_API_MESSAGE(StorageCommandFileClose);
    S_API_EPILOGUE;
    return S_RETURN_BOOL;
}

uint16_t s_api_file_read(void* context, File* file, void* buff, uint16_t bytes_to_read) {
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

uint16_t s_api_file_write(void* context, File* file, const void* buff, uint16_t bytes_to_write) {
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

bool s_api_file_seek(void* context, File* file, uint32_t offset, bool from_start) {
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

uint64_t s_api_file_tell(void* context, File* file) {
    S_API_PROLOGUE;
    S_API_DATA_FILE;
    S_API_MESSAGE(StorageCommandFileTell);
    S_API_EPILOGUE;
    return S_RETURN_UINT64;
}

bool s_api_file_truncate(void* context, File* file) {
    S_API_PROLOGUE;
    S_API_DATA_FILE;
    S_API_MESSAGE(StorageCommandFileTruncate);
    S_API_EPILOGUE;
    return S_RETURN_BOOL;
}

uint64_t s_api_file_size(void* context, File* file) {
    S_API_PROLOGUE;
    S_API_DATA_FILE;
    S_API_MESSAGE(StorageCommandFileSize);
    S_API_EPILOGUE;
    return S_RETURN_UINT64;
}

bool s_api_file_sync(void* context, File* file) {
    S_API_PROLOGUE;
    S_API_DATA_FILE;
    S_API_MESSAGE(StorageCommandFileSync);
    S_API_EPILOGUE;
    return S_RETURN_BOOL;
}

bool s_api_file_eof(void* context, File* file) {
    S_API_PROLOGUE;
    S_API_DATA_FILE;
    S_API_MESSAGE(StorageCommandFileEof);
    S_API_EPILOGUE;
    return S_RETURN_BOOL;
}

/****************** DIR ******************/

bool s_api_dir_open(void* context, File* file, const char* path) {
    S_API_PROLOGUE;

    SAData data = {
        .dopen = {
            .file = file,
            .path = path,
        }};

    S_API_MESSAGE(StorageCommandFirOpen);
    S_API_EPILOGUE;
    return S_RETURN_BOOL;
}

bool s_api_dir_close(void* context, File* file) {
    S_API_PROLOGUE;
    S_API_DATA_FILE;
    S_API_MESSAGE(StorageCommandDirClose);
    S_API_EPILOGUE;
    return S_RETURN_BOOL;
}

bool s_api_dir_read(
    void* context,
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

bool s_api_dir_rewind(void* context, File* file) {
    S_API_PROLOGUE;
    S_API_DATA_FILE;
    S_API_MESSAGE(StorageCommandDirRewind);
    S_API_EPILOGUE;
    return S_RETURN_BOOL;
}

/****************** COMMON ******************/

FS_Error s_api_common_info(
    void* context,
    const char* path,
    FileInfo* fileinfo,
    char* name,
    const uint16_t name_length) {
    S_API_PROLOGUE;

    SAData data = {
        .cinfo = {
            .path = path,
            .fileinfo = fileinfo,
            .name = name,
            .name_length = name_length,
        }};

    S_API_MESSAGE(StorageCommandCommonInfo);
    S_API_EPILOGUE;
    return S_RETURN_ERROR;
}

FS_Error s_api_common_remove(void* context, const char* path) {
    S_API_PROLOGUE;
    S_API_DATA_PATH;
    S_API_MESSAGE(StorageCommandCommonRemove);
    S_API_EPILOGUE;
    return S_RETURN_ERROR;
}

FS_Error s_api_common_rename(void* context, const char* old_path, const char* new_path) {
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

FS_Error s_api_common_mkdir(void* context, const char* path) {
    S_API_PROLOGUE;
    S_API_DATA_PATH;
    S_API_MESSAGE(StorageCommandCommonMkDir);
    S_API_EPILOGUE;
    return S_RETURN_ERROR;
}

FS_Error s_api_common_get_fs_info(
    void* context,
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

    S_API_MESSAGE(StorageCommandCommonGetFSInfo);
    S_API_EPILOGUE;
    return S_RETURN_ERROR;
}

/****************** ERROR ******************/

const char* s_api_error_get_desc(void* context, FS_Error error_id) {
    S_API_PROLOGUE;
    SAData data = {
        .error = {
            .id = error_id,
        }};
    S_API_MESSAGE(StorageCommandErrorGetDesc);
    S_API_EPILOGUE;
    return S_RETURN_CSTRING;
}

const char* s_api_error_get_internal_desc(void* context, uint32_t internal_error_id) {
    S_API_PROLOGUE;
    SAData data = {
        .error = {
            .id = internal_error_id,
        }};
    S_API_MESSAGE(StorageCommandCommonGetInternalDesc);
    S_API_EPILOGUE;
    return S_RETURN_CSTRING;
}
