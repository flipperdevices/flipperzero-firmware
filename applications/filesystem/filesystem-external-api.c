#include "filesystem-i.h"
#include "filesystem-message.h"

static FilesystemApp* app;

#define FS_API_PROLOGUE                           \
    osThreadId_t caller_thread = osThreadGetId(); \
    if(caller_thread == 0) furi_check(0);

#define FS_API_EPILOGUE                                                                    \
    furi_check(osMessageQueuePut(app->message_queue, &message, 0, osWaitForever) == osOK); \
    osThreadFlagsWait(FILESYSTEM_THREAD_FLAG_COMPLETE, osFlagsWaitAny, osWaitForever);

#define FS_API_MESSAGE(_command)     \
    FSAReturn return_data;           \
    FilesystemMessage message = {    \
        .thread = caller_thread,     \
        .command = _command,         \
        .data = &data,               \
        .return_data = &return_data, \
    };

#define FS_API_DATA_FILE  \
    FSAData data = {      \
        .file = {         \
            .file = file, \
        }};

#define FS_API_DATA_PATH  \
    FSAData data = {      \
        .path = {         \
            .path = path, \
        }};

#define FS_RETURN_BOOL (return_data.bool_value);
#define FS_RETURN_UINT16 (return_data.uint16_value);
#define FS_RETURN_UINT64 (return_data.uint64_value);
#define FS_RETURN_ERROR (return_data.error_value);
#define FS_RETURN_CSTRING (return_data.cstring_value);

void fs_api_file_set_filesystem_app(FilesystemApp* filesystem_app) {
    app = filesystem_app;
}

/****************** FILE ******************/

bool fs_api_file_open(
    File* file,
    const char* path,
    FS_AccessMode access_mode,
    FS_OpenMode open_mode) {
    FS_API_PROLOGUE;

    FSAData data = {
        .fopen = {
            .file = file,
            .path = path,
            .access_mode = access_mode,
            .open_mode = open_mode,
        }};

    FS_API_MESSAGE(FC_FILE_OPEN);
    FS_API_EPILOGUE;
    return FS_RETURN_BOOL;
}

bool fs_api_file_close(File* file) {
    FS_API_PROLOGUE;
    FS_API_DATA_FILE;
    FS_API_MESSAGE(FC_FILE_CLOSE);
    FS_API_EPILOGUE;
    return FS_RETURN_BOOL;
}

uint16_t fs_api_file_read(File* file, void* buff, uint16_t bytes_to_read) {
    FS_API_PROLOGUE;

    FSAData data = {
        .fread = {
            .file = file,
            .buff = buff,
            .bytes_to_read = bytes_to_read,
        }};

    FS_API_MESSAGE(FC_FILE_READ);
    FS_API_EPILOGUE;
    return FS_RETURN_UINT16;
}

uint16_t fs_api_file_write(File* file, const void* buff, uint16_t bytes_to_write) {
    FS_API_PROLOGUE;

    FSAData data = {
        .fwrite = {
            .file = file,
            .buff = buff,
            .bytes_to_write = bytes_to_write,
        }};

    FS_API_MESSAGE(FC_FILE_WRITE);
    FS_API_EPILOGUE;
    return FS_RETURN_UINT16;
}

bool fs_api_file_seek(File* file, uint32_t offset, bool from_start) {
    FS_API_PROLOGUE;

    FSAData data = {
        .fseek = {
            .file = file,
            .offset = offset,
            .from_start = from_start,
        }};

    FS_API_MESSAGE(FC_FILE_SEEK);
    FS_API_EPILOGUE;
    return FS_RETURN_BOOL;
}

uint64_t fs_api_file_tell(File* file) {
    FS_API_PROLOGUE;
    FS_API_DATA_FILE;
    FS_API_MESSAGE(FC_FILE_TELL);
    FS_API_EPILOGUE;
    return FS_RETURN_UINT64;
}

bool fs_api_file_truncate(File* file) {
    FS_API_PROLOGUE;
    FS_API_DATA_FILE;
    FS_API_MESSAGE(FC_FILE_TRUNCATE);
    FS_API_EPILOGUE;
    return FS_RETURN_BOOL;
}

uint64_t fs_api_file_size(File* file) {
    FS_API_PROLOGUE;
    FS_API_DATA_FILE;
    FS_API_MESSAGE(FC_FILE_SIZE);
    FS_API_EPILOGUE;
    return FS_RETURN_UINT64;
}

bool fs_api_file_sync(File* file) {
    FS_API_PROLOGUE;
    FS_API_DATA_FILE;
    FS_API_MESSAGE(FC_FILE_SYNC);
    FS_API_EPILOGUE;
    return FS_RETURN_BOOL;
}

bool fs_api_file_eof(File* file) {
    FS_API_PROLOGUE;
    FS_API_DATA_FILE;
    FS_API_MESSAGE(FC_FILE_EOF);
    FS_API_EPILOGUE;
    return FS_RETURN_BOOL;
}

/****************** DIR ******************/

bool fs_api_dir_open(File* file, const char* path) {
    FS_API_PROLOGUE;

    FSAData data = {
        .dopen = {
            .file = file,
            .path = path,
        }};

    FS_API_MESSAGE(FC_DIR_OPEN);
    FS_API_EPILOGUE;
    return FS_RETURN_BOOL;
}

bool fs_api_dir_close(File* file) {
    FS_API_PROLOGUE;
    FS_API_DATA_FILE;
    FS_API_MESSAGE(FC_DIR_CLOSE);
    FS_API_EPILOGUE;
    return FS_RETURN_BOOL;
}

bool fs_api_dir_read(File* file, FileInfo* fileinfo, char* name, uint16_t name_length) {
    FS_API_PROLOGUE;

    FSAData data = {
        .dread = {
            .file = file,
            .fileinfo = fileinfo,
            .name = name,
            .name_length = name_length,
        }};

    FS_API_MESSAGE(FC_DIR_READ);
    FS_API_EPILOGUE;
    return FS_RETURN_BOOL;
}

bool fs_api_dir_rewind(File* file) {
    FS_API_PROLOGUE;
    FS_API_DATA_FILE;
    FS_API_MESSAGE(FC_DIR_REWIND);
    FS_API_EPILOGUE;
    return FS_RETURN_BOOL;
}

/****************** COMMON ******************/

FS_Error fs_api_common_info(
    const char* path,
    FileInfo* fileinfo,
    char* name,
    const uint16_t name_length) {
    FS_API_PROLOGUE;

    FSAData data = {
        .cinfo = {
            .path = path,
            .fileinfo = fileinfo,
            .name = name,
            .name_length = name_length,
        }};

    FS_API_MESSAGE(FC_COMMON_INFO);
    FS_API_EPILOGUE;
    return FS_RETURN_ERROR;
}

FS_Error fs_api_common_remove(const char* path) {
    FS_API_PROLOGUE;
    FS_API_DATA_PATH;
    FS_API_MESSAGE(FC_COMMON_REMOVE);
    FS_API_EPILOGUE;
    return FS_RETURN_ERROR;
}

FS_Error fs_api_common_rename(const char* old_path, const char* new_path) {
    FS_API_PROLOGUE;

    FSAData data = {
        .crename = {
            .old_path = old_path,
            .new_path = new_path,
        }};

    FS_API_MESSAGE(FC_COMMON_RENAME);
    FS_API_EPILOGUE;
    return FS_RETURN_ERROR;
}

FS_Error fs_api_common_mkdir(const char* path) {
    FS_API_PROLOGUE;
    FS_API_DATA_PATH;
    FS_API_MESSAGE(FC_COMMON_MKDIR);
    FS_API_EPILOGUE;
    return FS_RETURN_ERROR;
}

FS_Error fs_api_common_get_fs_info(uint64_t* total_space, uint64_t* free_space) {
    return FSE_NOT_IMPLEMENTED;
}

/****************** ERROR ******************/

const char* fs_api_error_get_desc(FS_Error error_id) {
    FS_API_PROLOGUE;
    FSAData data = {
        .error = {
            .id = error_id,
        }};
    FS_API_MESSAGE(FC_ERROR_GET_DESC);
    FS_API_EPILOGUE;
    return FS_RETURN_CSTRING;
}

const char* fs_api_error_get_internal_desc(uint32_t internal_error_id) {
    FS_API_PROLOGUE;
    FSAData data = {
        .error = {
            .id = internal_error_id,
        }};
    FS_API_MESSAGE(FC_ERROR_GET_INTERNAL_DESC);
    FS_API_EPILOGUE;
    return FS_RETURN_CSTRING;
}
