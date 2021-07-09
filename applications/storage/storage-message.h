#pragma once
#include <furi.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STORAGE_THREAD_FLAG_COMPLETE (1)

typedef struct {
    File* file;
    const char* path;
    FS_AccessMode access_mode;
    FS_OpenMode open_mode;
} SADataFOpen;

typedef struct {
    File* file;
    void* buff;
    uint16_t bytes_to_read;
} SADataFRead;

typedef struct {
    File* file;
    const void* buff;
    uint16_t bytes_to_write;
} SADataFWrite;

typedef struct {
    File* file;
    uint32_t offset;
    bool from_start;
} SADataFSeek;

typedef struct {
    File* file;
    const char* path;
} SADataDOpen;

typedef struct {
    File* file;
    FileInfo* fileinfo;
    char* name;
    uint16_t name_length;
} SADataDRead;

typedef struct {
    const char* path;
    FileInfo* fileinfo;
    char* name;
    const uint16_t name_length;
} SADataCInfo;

typedef struct {
    const char* old_path;
    const char* new_path;
} SADataCRename;

typedef struct {
    const char* fs_path;
    uint64_t* total_space;
    uint64_t* free_space;
} SADataCFSInfo;

typedef struct {
    uint32_t id;
} SADataError;

typedef struct {
    const char* path;
} SADataPath;

typedef struct {
    File* file;
} SADataFile;

typedef union {
    SADataFOpen fopen;
    SADataFRead fread;
    SADataFWrite fwrite;
    SADataFSeek fseek;

    SADataDOpen dopen;
    SADataDRead dread;

    SADataCInfo cinfo;
    SADataCRename crename;
    SADataCFSInfo cfsinfo;

    SADataError error;

    SADataFile file;
    SADataPath path;
} SAData;

typedef union {
    bool bool_value;
    uint16_t uint16_value;
    uint64_t uint64_value;
    FS_Error error_value;
    const char* cstring_value;
} SAReturn;

typedef enum {
    SC_FILE_OPEN,
    SC_FILE_CLOSE,
    SC_FILE_READ,
    SC_FILE_WRITE,
    SC_FILE_SEEK,
    SC_FILE_TELL,
    SC_FILE_TRUNCATE,
    SC_FILE_SIZE,
    SC_FILE_SYNC,
    SC_FILE_EOF,
    SC_DIR_OPEN,
    SC_DIR_CLOSE,
    SC_DIR_READ,
    SC_DIR_REWIND,
    SC_COMMON_INFO,
    SC_COMMON_REMOVE,
    SC_COMMON_RENAME,
    SC_COMMON_MKDIR,
    SC_COMMON_GET_FS_INFO,
    SC_ERROR_GET_DESC,
    SC_ERROR_GET_INTERNAL_DESC,
} StorageCommand;

typedef struct {
    osThreadId thread;
    StorageCommand command;
    SAData* data;
    SAReturn* return_data;
} StorageMessage;

#ifdef __cplusplus
}
#endif